// Copyright 2019 Google LLC
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
// See the License for the specific language governing permissions and
// limitations under the License.
#include <csignal>
#include <cstdlib>

#include "glog/logging.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "cxx/internal/grpc/grpc.h"
#include "cxx/internal/queue.h"
#include "go/internal/quickstore_microservice/quickstore_service.h"

ABSL_FLAG(std::string, addr, "localhost:9813",
          "Address on which to run the Quickstore microservice.");

constexpr char kDefaultHost[] = "https://mako.dev";

static void sigdown(int signo) {
  psignal(signo, "Shutting down, got signal");
  std::exit(0);
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  // Without a PID1 or shell, we need to handle SIGINT on our own.
  struct sigaction down_handler;

  // Initialize all args to avoid undefined behavior.
  down_handler.sa_flags = 0;
  sigemptyset(&down_handler.sa_mask);
  down_handler.sa_handler = sigdown;

  if (sigaction(SIGINT, &down_handler, nullptr) < 0) {
    return 1;
  }

  absl::string_view default_host = kDefaultHost;
  auto env_var_default_host =
      absl::NullSafeStringView(std::getenv("MAKO_SERVER_ADDRESS"));
  if (!env_var_default_host.empty()) {
    LOG(INFO) << "Found MAKO_SERVER_ADDRESS set to \"" << env_var_default_host
              << "\". Overriding default address of \"" << default_host << "\"";
    default_host = env_var_default_host;
  }

  LOG(INFO)
      << "Quickstore will by default connect to the target Mako server at "
      << default_host << ". The target server may be overridden by a client.";

  grpc::ServerBuilder builder;
  builder.SetMaxReceiveMessageSize(std::numeric_limits<int32_t>::max());
  builder.SetMaxSendMessageSize(std::numeric_limits<int32_t>::max());
  builder.AddListeningPort(
      absl::GetFlag(FLAGS_addr),
      grpc::InsecureServerCredentials());  //  NOLINT

  mako::internal::Queue<bool> shutdown_queue;
  auto service =
      mako::internal::quickstore_microservice::QuickstoreService::Create(
          std::string(default_host), &shutdown_queue)
          .value();
  builder.RegisterService(service.get());

  auto server = builder.BuildAndStart();

  // Block until we get an RPC that tells us to shut down.
  shutdown_queue.get();
  LOG(INFO) << "Shutdown message received. Quickstore microservice down.";
}
