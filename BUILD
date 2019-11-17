# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("@com_github_chokobole_felicia//bazel:felicia_cc.bzl", "fel_cc_native_library")

exports_files([
    "LICENSE",
])

fel_cc_native_library(
    name = "hector_slam",
    srcs = [
        "hector_slam/hector_slam.cc",
        "hector_slam/log_odds_occupancy_grid_map.cc",
        "hector_slam/multi_resolution_grid_map.cc",
        "hector_slam/point_container.cc",
    ],
    hdrs = [
        "hector_slam/hector_slam.h",
        "hector_slam/log_odds_occupancy_grid_map.h",
        "hector_slam/multi_resolution_grid_map.h",
        "hector_slam/point_container.h",
        "hector_slam/scan_matcher.h",
    ],
    deps = [
        "//external:felicia",
    ],
    visibility = ["//visibility:public"],
)
