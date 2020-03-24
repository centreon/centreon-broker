/*
 * Copyright 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <iostream>
#include <memory>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include "../src/broker.grpc.pb.h"

class BrokerRPCClient {
  std::unique_ptr<Broker::Stub> _stub;

 public:
  BrokerRPCClient(std::shared_ptr<grpc::Channel> channel)
      : _stub(Broker::NewStub(channel)) {}

  bool GetVersion(Version* version) {
    const ::google::protobuf::Empty e;
    grpc::ClientContext context;
    grpc::Status status = _stub->GetVersion(&context, e, version);
    if (!status.ok()) {
      std::cout << "GetVersion rpc failed." << std::endl;
      return false;
    }
    return true;
  }

  bool DebugConfReload(GenericResponse* response, std::string const& file) {
    GenericString request;
    request.set_allocated_str_arg(new std::string(file));

    grpc::ClientContext context;
    grpc::Status status = _stub->DebugConfReload(&context, request, response);
    if (!status.ok()) {
      std::cout << "ConfReload rpc failed." << std::endl;
      return false;
    }

    if (response->ok())
      return true;

    return false;
  }
};

int main(int argc, char** argv) {
  int32_t status;
  BrokerRPCClient client(grpc::CreateChannel(
      "127.0.0.1:50052", grpc::InsecureChannelCredentials()));

  if (argc < 2) {
    std::cout << "ERROR: this client must be called with a command..."
              << std::endl;
    exit(1);
  }

  if (strcmp(argv[1], "GetVersion") == 0) {
    Version version;
    status = client.GetVersion(&version) ? 0 : 1;
    std::cout << "GetVersion: " << version.DebugString();
  }
  else if (strcmp(argv[1], "DebugConfReload") == 0) {
    GenericResponse response;
    status = client.DebugConfReload(&response, argv[2]) ? 0 : 1;
    if (!response.ok())
      std::cout << "DebugConfReload failed for file " << argv[2] << " : " << response.err_msg() << std::endl;
    else
      std::cout << "DebugConfReload OK" << std::endl;
  }

  exit(status);
}
