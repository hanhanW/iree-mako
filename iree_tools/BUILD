licenses(["notice"])

cc_binary(
    name = "uploader",
    srcs = ["uploader.cc"],
    tags = [
        "manual",
        "notap",
    ],
    deps = [
        ":perf_data_cc_proto",
        "//cxx/clients/analyzers:threshold_analyzer",
        "//cxx/quickstore",
        "//proto/clients/analyzers:threshold_analyzer_cc_proto",
        "//proto/quickstore:quickstore_cc_proto",
        "//spec/proto:mako_cc_proto",
        "@com_google_absl//absl/flags:parse",
        "@com_google_protobuf//:protobuf",
    ],
)

proto_library(
    name = "perf_data_proto",
    srcs = ["perf_data.proto"],
)

cc_proto_library(
    name = "perf_data_cc_proto",
    deps = [":perf_data_proto"],
)
