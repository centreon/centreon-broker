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

#include "com/centreon/broker/broker_impl.hh"

#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/version.hh"
#include "com/centreon/broker/stats

using namespace com::centreon::broker;
using namespace com::centreon::broker::version;

/**
 * @brief Return the Broker's version.
 *
 * @param context gRPC context
 * @param  unused
 * @param response A Version object to fill
 *
 * @return Status::OK
 */
grpc::Status broker_impl::GetVersion(grpc::ServerContext* context,
                                     const ::google::protobuf::Empty* request,
                                     Version* response) {
  response->set_major(major);
  response->set_minor(minor);
  response->set_patch(patch);
  return grpc::Status::OK;
}
grpc::Status broker_impl::DebugConfReload(grpc::ServerContext* context,
                                          const GenericString* request,
                                          GenericResponse* response) {
  std::string err;
  if (log_v2::instance().load(request->str_arg(), _broker_name, err)) {
    response->set_ok(true);
    response->set_err_msg("");
  } else {
    response->set_ok(false);
    response->set_err_msg(std::move(err));
  }

  return grpc::Status::OK;
}

grpc::Status broker_impl::GetNumModules(grpc::ServerContext* context,
                                        const ::google::protobuf::Empty*,
                                        GenericSize* response) {
  config::applier::modules& mod_applier(config::applier::modules::instance());

  std::lock_guard<std::mutex> lock(mod_applier.module_mutex());
  response->set_size(std::distance(mod_applier.begin(), mod_applier.end()));

  return grpc::Status::OK;
}

grpc::Status broker_impl::GetNumEndpoint(grpc::ServerContext* context,
                                         const ::google::protobuf::Empty*,
                                         GenericSize* response) {
  // Endpoint applier.
  config::applier::endpoint& endp_applier(
      config::applier::endpoint::instance());

  std::lock_guard<std::timed_mutex> lock(endp_applier.endpoints_mutex());
  response->set_size(std::distance(endp_applier.endpoints_begin(),
                                   endp_applier.endpoints_end()));

  return grpc::Status::OK;
}
grpc::Status broker_impl::GetModulesStats(grpc::ServerContext* context,
                                          const GenericNameOrIndex* request,
                                          GenericString* response) {
  config::applier::modules& mod_applier(config::applier::modules::instance());

  std::lock_guard<std::mutex> lock(mod_applier.module_mutex());

  std::vector<json11::JsonValue> object;

  switch (request->nameOrIndex_case()) {
    case GenericNameOrIndex::kIdx:
      break;
    case GenericNameOrIndex::kStr:
      break;
    case GenericNameOrIndex::NAMEORINDEX_NOT_SET:
      break;
  }

  return Service::GetModulesStats(context, request, response);
}
grpc::Status broker_impl::GetEndpointStats(grpc::ServerContext* context,
                                           const GenericNameOrIndex* request,
                                           GenericString* response) {
  return Service::GetEndpointStats(context, request, response);
}
