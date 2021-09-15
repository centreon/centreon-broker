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

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <iostream>
#include <memory>
#include "../src/broker.grpc.pb.h"

using namespace com::centreon::broker;

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

  bool GetSqlConnectionStats(BrokerStats* response) {
    const ::google::protobuf::Empty e;
    grpc::ClientContext context;
    grpc::Status status = _stub->GetSqlConnectionStats(&context, e, response);
    if (!status.ok()) {
      std::cout << "GetSqlConnectionStats rpc failed." << std::endl;
      return false;
    }
    return true;
  }

  bool GetConflictManagerStats(BrokerStats* response) {
    const ::google::protobuf::Empty e;
    grpc::ClientContext context;
    grpc::Status status = _stub->GetSqlConnectionStats(&context, e, response);
    if (!status.ok()) {
      std::cout << "GetConflictManager rpc failed." << std::endl;
      return false;
    }
    return true;
  }


  
};

int main(int argc, char** argv) {
  int32_t status = 0;
  BrokerRPCClient client(grpc::CreateChannel(
      "127.0.0.1:40000", grpc::InsecureChannelCredentials()));

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

  if (strcmp(argv[1], "GetSqlConnectionStatsSize") == 0) {
    BrokerStats response;
    status = client.GetSqlConnectionStats(&response) ? 0 : 1;

    std::cout << "connection size: "
              << response.mutable_connections()->size()
              << std::endl;
  }

  if (strcmp(argv[1], "GetSqlConnectionStatsValue") == 0) {
    BrokerStats response;
    status = client.GetSqlConnectionStats(&response) ? 0 : 1;
    for (auto
             it = response.mutable_connections()->begin(),
             end = response.mutable_connections()->end();
             it != end; ++it) {
      std::cout << (*it).waiting_tasks() << std::endl; 
      }
  }

  if (strcmp(argv[1], "GetConflictManagerStats") == 0) {
    BrokerStats response;
    status = client.GetConflictManagerStats(&response) ? 0 : 1;
  }

  exit(status);
}
