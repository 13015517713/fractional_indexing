cc_library(
  name = "fractional_indexing",
  srcs = [
    "fractional_indexing.cpp",
  ],
  hdrs = [
    "fractional_indexing.h",
  ],
  includes = ["."],
  copts = [
    "-std=c++11",
  ],
  linkopts = [],
  visibility = ["//visibility:public"],
)

cc_binary(
  name = "fractional_indexing_test",
  srcs = [
    "fractional_indexing_test.cpp",
  ],
  includes = ["."],
  copts = [
      "-std=c++11",
  ],
  linkopts = [],
  deps = [
    ":fractional_indexing",
    # ":gtest", # need to set self
    # ":gtest_main",
  ],
)
