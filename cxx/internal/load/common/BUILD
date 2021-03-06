# C++ Common Load components
package(default_visibility = ["//:internal"])

licenses(["notice"])

cc_library(
    name = "executor",
    hdrs = ["executor.h"],
    visibility = [
        "//:internal",
        "//helpers/cxx/streamz_metric_collector:__pkg__",
    ],
    deps = [
        ":thread_pool_factory",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "executor_test",
    size = "small",
    srcs = ["executor_test.cc"],
    deps = [
        ":executor",
        "@com_google_absl//absl/synchronization",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "thread_pool",
    hdrs = ["thread_pool.h"],
    visibility = ["//visibility:private"],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "thread_pool_test",
    srcs = ["thread_pool_test.cc"],
    deps = [
        ":thread_pool",
        "@com_google_absl//absl/synchronization",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "thread_pool_factory",
    srcs = ["thread_pool_factory.cc"],
    hdrs = ["thread_pool_factory.h"],
    deps = [
        ":thread_pool",
        "@com_google_absl//absl/memory",
    ],
)

cc_library(
    name = "run_analyzers",
    srcs = ["run_analyzers.cc"],
    hdrs = ["run_analyzers.h"],
    deps = [
        "//cxx/helpers/status",
        "//cxx/internal:analyzer_optimizer",
        "//cxx/spec:aggregator",
        "//cxx/spec:analyzer",
        "//cxx/spec:dashboard",
        "//cxx/spec:storage",
        "//proto/internal:mako_internal_cc_proto",
        "//spec/proto:mako_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "run_analyzers_test",
    size = "small",
    srcs = ["run_analyzers_test.cc"],
    deps = [
        ":run_analyzers",
        "//cxx/clients/analyzers:threshold_analyzer",
        "//cxx/clients/dashboard:standard_dashboard",
        "//cxx/clients/storage:fake_google3_storage",
        "//cxx/spec:analyzer",
        "//cxx/spec:dashboard",
        "//cxx/spec:mock_analyzer",
        "//cxx/spec:storage",
        "//cxx/testing:protocol-buffer-matchers",
        "//proto/clients/analyzers:threshold_analyzer_cc_proto",
        "//proto/internal:mako_internal_cc_proto",
        "//spec/proto:mako_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest_main",
    ],
)
