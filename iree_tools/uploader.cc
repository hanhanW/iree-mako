// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// see the license for the specific language governing permissions and
// limitations under the license.

#include <fstream>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "cxx/clients/analyzers/threshold_analyzer.h"
#include "cxx/quickstore/quickstore.h"
#include "glog/logging.h"
#include "iree_tools/perf_data.pb.h"
#include "proto/clients/analyzers/threshold_analyzer.pb.h"
#include "proto/quickstore/quickstore.pb.h"
#include "spec/proto/mako.pb.h"
#include "src/google/protobuf/text_format.h"

ABSL_FLAG(std::string, file_path, "", "The path to the proto data to upload.");

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  // STEP 1: Collect some performance data from file_path.
  iree_mako::PerfData data;
  std::string filepath = absl::GetFlag(FLAGS_file_path);
  std::ifstream t(filepath);
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  if (!google::protobuf::TextFormat::ParseFromString(str, &data)) {
    LOG(ERROR) << "Failed to parse text protobuf from file: " << filepath;
  }

  // STEP 2: Configure run metadata in QuickstoreInput.
  mako::quickstore::QuickstoreInput quickstore_input;
  quickstore_input.set_benchmark_key(data.metadata().benchmark_key());
  quickstore_input.set_timestamp_ms(data.metadata().timestamp_ms());
  quickstore_input.set_hover_text(data.metadata().git_hash());
  quickstore_input.mutable_tags()->CopyFrom(data.metadata().tags());

  // STEP 3: Configure an Analyzer
  //
  // Read more about analyzers at
  // https://github.com/google/mako/blob/master/docs/ANALYZERS.md
  auto add_wda = [&](const std::string& key) {
    mako::window_deviation::WindowDeviationInput* wda_input =
        quickstore_input.add_wda_inputs();
    auto* wda_query = wda_input->add_run_info_query_list();
    wda_query->set_benchmark_key(data.metadata().benchmark_key());
    wda_query->set_limit(14);
    auto* wda_check = wda_input->add_tolerance_check_list();
    wda_check->mutable_data_filter()->set_data_type(
        mako::DataFilter::METRIC_AGGREGATE_MEDIAN);
    wda_check->mutable_data_filter()->set_value_key(key);
    wda_check->set_recent_window_size(5);
    wda_check->set_direction_bias(
        mako::window_deviation::ToleranceCheck::IGNORE_DECREASE);
    wda_check->add_median_tolerance_params_list()->set_median_coeff(0.05);
  };
  add_wda("cpu");
  add_wda("vmla");
  add_wda("vlk");

  // STEP 4: Create a Quickstore instance which reports to the Mako server
  //
  // Read about setting up authentication at
  // http://github.com/google/mako/blob/master/docs/GUIDE.md#setting-up-authentication
  mako::quickstore::Quickstore quickstore(quickstore_input);

  // STEP 5: Feed sample point data to the Mako Quickstore client.
  for (const auto& sample : data.samples()) {
    std::map<std::string, double> metrics;
    metrics[sample.target()] = sample.time();
    quickstore.AddSamplePoint(data.metadata().timestamp_ms(), metrics);
  }

  // STEP 6: Feed your aggregate data to the Mako Quickstore client.
  // Skip this step for now.

  // STEP 7: Call Store() to instruct Mako to process the data and upload it to
  // http://mako.dev.
  mako::quickstore::QuickstoreOutput output = quickstore.Store();
  switch (output.status()) {
    case mako::quickstore::QuickstoreOutput::SUCCESS:
      LOG(INFO) << " Done! Run can be found at: " << output.run_chart_link();
      break;
    case mako::quickstore::QuickstoreOutput::ERROR:
      LOG(ERROR) << "Quickstore Store error: " << output.summary_output();
      return 1;
    case mako::quickstore::QuickstoreOutput::ANALYSIS_FAIL:
      LOG(ERROR) << "Quickstore Analysis Failure: " << output.summary_output()
                 << "\nRun can be found at: " << output.run_chart_link();
      return 1;
  }

  return 0;
}
