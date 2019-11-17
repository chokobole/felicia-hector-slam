# Felicia Hector SLAM

This is taken and modified from [Hector SLAM](https://github.com/tu-darmstadt-ros-pkg/hector_slam).

## WORKSPACE setup

```python
bind(
    name = "felicia",
    actual = "@com_github_chokobole_felicia//felicia:felicia",
)

bind(
    name = "felicia_hector_slam",
    actual = "@com_github_chokobole_felicia_hector_slam//:hector_slam",
)

git_repository(
    name = "com_github_chokobole_felicia",
    remote = "https://github.com/chokobole/felicia.git",
    commit = "<commit>",
)

git_repository(
    name = "com_github_chokobole_felicia_hector_slam",
    remote = "https://github.com/chokobole/felicia-hector-slam.git",
    commit = "<commit>",
)

load("@com_github_chokobole_felicia//bazel:felicia_deps.bzl", "felicia_deps")

felicia_deps()
```

## Example

```python
load("@com_github_chokobole_felicia//bazel:felicia_cc.bzl", "fel_cc_native_binary")

fel_cc_native_binary(
    name = "<name>",
    ...,
    deps = [
        "//external:felicia_hector_slam",
    ],
)
```

Check out examples [here](https://github.com/chokobole/felicia-examples/blob/master/examples/slam/hector_slam/BUILD).