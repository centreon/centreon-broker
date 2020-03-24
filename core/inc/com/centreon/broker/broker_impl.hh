/*
 * Copyright 2020 Centreon (https://www.centreon.com/)
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
#ifndef CENTREON_BROKER_CORE_SRC_BROKERIMPL_HH_
#define CENTREON_BROKER_CORE_SRC_BROKERIMPL_HH_

#include "com/centreon/broker/namespace.hh"
#include "broker.grpc.pb.h"

CCB_BEGIN()
class broker_impl final : public Broker::Service {
  std::string _broker_name;

  grpc::Status GetVersion(grpc::ServerContext* context,
                          const ::google::protobuf::Empty* /*request*/,
                          Version* response) override;

  grpc::Status DebugConfReload(grpc::ServerContext* context,
                          const GenericString* /*request*/,
                          GenericResponse* response) override;

  grpc::Status GetNumModules(grpc::ServerContext* context,
                          const ::google::protobuf::Empty* /*request*/,
                          GenericSize* response) override;

  grpc::Status GetModulesStats(grpc::ServerContext* context,
                               const GenericNameOrIndex* request,
                               GenericString* response) override;


  grpc::Status GetNumEndpoint(grpc::ServerContext* context,
                             const ::google::protobuf::Empty* /*request*/,
                             GenericSize* response) override;


  grpc::Status GetEndpointStats(grpc::ServerContext* context,
                               const GenericNameOrIndex* request,
                               GenericString* response) override;

 public:
  void set_broker_name(std::string const& s) {_broker_name = s;};
};
CCB_END()

#endif  // CENTREON_BROKER_CORE_SRC_BROKERIMPL_HH_
