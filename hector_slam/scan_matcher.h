//=================================================================================================
// Copyright (c) 2011, Stefan Kohlbrecher, TU Darmstadt
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Simulation, Systems Optimization and Robotics
//       group, TU Darmstadt nor the names of its contributors may be used to
//       endorse or promote products derived from this software without
//       specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//=================================================================================================

#ifndef HECTOR_SLAM_SCAN_MATCHER_H_
#define HECTOR_SLAM_SCAN_MATCHER_H_

#include "chromium/base/optional.h"
#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/lib/unit/geometry/transform.h"

namespace felicia {
namespace hector_slam {

template <typename MapType>
class ScanMatcher {
 public:
  // Given world coordinate |pose|, estimate the world coordinate |pose|
  // until |max_iterations| where matches the most |points|.
  Posef Match(const Posef& pose, const std::vector<Pointf>& points,
              MapType* map, int max_iterations);

 private:
  base::Optional<Posef> Estimate(const Posef& pose,
                                 const std::vector<Pointf>& points,
                                 MapType* map);
};

template <typename MapType>
Posef ScanMatcher<MapType>::Match(const Posef& pose,
                                  const std::vector<Pointf>& points,
                                  MapType* map, int max_iterations) {
  if (points.size() == 0) return pose;

  Posef estimated{map->ToMapCoordinate(pose.position()), pose.theta()};
  for (int i = 0; i <= max_iterations; ++i) {
    base::Optional<Posef> new_estimated = Estimate(estimated, points, map);
    if (!new_estimated.has_value()) break;
    estimated = new_estimated.value();
  }

  return {map->ToWorldCoordinate(estimated.position()),
          normalize_angle(estimated.theta(), 0.f, k2PiFloat)};
}

template <typename MapType>
base::Optional<Posef> ScanMatcher<MapType>::Estimate(
    const Posef& pose, const std::vector<Pointf>& points, MapType* map) {
  // Please refer to section 4 from
  // http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.302.2579&rep=rep1&type=pdf
  Transformf transform = pose.ToTransform();

  float sin_rot = sin(pose.theta());
  float cos_rot = cos(pose.theta());

  Eigen::Matrix3f H = Eigen::Matrix3f::Zero();
  Eigen::Vector3f dTr = Eigen::Vector3f::Zero();

  for (const Pointf& point : points) {
    float values[3];
    Pointf transformed = point.Transform(transform);
    map->InterpolatedValue(transformed.x(), transformed.y(), &(values[0]),
                           true);

    float x_deriv = values[1];
    float y_deriv = values[2];
    float rot_deriv = (-sin_rot * point.x() - cos_rot * point.y()) * x_deriv +
                      (cos_rot * point.x() - sin_rot * point.y()) * y_deriv;
    float a = 1.f - values[0];

    dTr[0] += x_deriv * a;
    dTr[1] += y_deriv * a;
    dTr[2] += rot_deriv * a;

    H(0, 0) += x_deriv * x_deriv;
    H(1, 1) += y_deriv * y_deriv;
    H(2, 2) += rot_deriv * rot_deriv;

    H(0, 1) += x_deriv * y_deriv;
    H(0, 2) += x_deriv * rot_deriv;
    H(1, 2) += y_deriv * rot_deriv;
  }

  H(1, 0) = H(0, 1);
  H(2, 0) = H(0, 2);
  H(2, 1) = H(1, 2);

  if ((H(0, 0) != 0.0f) && (H(1, 1) != 0.0f)) {
    Eigen::Vector3f search_dir(H.inverse() * dTr);

    float x_delta = search_dir[0];
    float y_delta = search_dir[1];
    float theta_delta = clamp(search_dir[2], -0.2f, 0.2f);

    Posef estimated;
    estimated.set_x(pose.x() + x_delta);
    estimated.set_y(pose.y() + y_delta);
    estimated.set_theta(pose.theta() + theta_delta);

    return estimated;
  }

  return base::nullopt;
}

}  // namespace hector_slam
}  // namespace felicia

#endif  // HECTOR_SLAM_SCAN_MATCHER_H_