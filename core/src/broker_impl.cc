/*
 * Copyright 2020-2021 Centreon (https://www.centreon.com/)
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
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/stats/center.hh"
#include "com/centreon/broker/stats/helper.hh"
#include "com/centreon/broker/version.hh"

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

grpc::Status broker_impl::GetNumModules(grpc::ServerContext* context,
                                        const ::google::protobuf::Empty*,
                                        GenericSize* response) {
  auto& mod_applier(config::applier::state::instance().get_modules());

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
  std::vector<json11::Json::object> value;
  stats::get_loaded_module_stats(value);

  bool found{false};
  json11::Json val;
  json11::Json::object object;
  switch (request->nameOrIndex_case()) {
    case GenericNameOrIndex::NAMEORINDEX_NOT_SET:
      for (auto& obj : value) {
        object["module" + obj["name"].string_value()] = obj;
      }
      val = object;
      response->set_str_arg(std::move(val.dump()));
      break;

    case GenericNameOrIndex::kStr:
      for (auto& obj : value) {
        if (obj["name"].string_value() == request->str()) {
          found = true;
          val = obj;
          response->set_str_arg(std::move(val.dump()));
          break;
        }
      }
      if (!found)
        return grpc::Status(grpc::INVALID_ARGUMENT,
                            grpc::string("name not found"));

      break;

    case GenericNameOrIndex::kIdx:

      if (request->idx() + 1 > value.size())
        return grpc::Status(grpc::INVALID_ARGUMENT,
                            grpc::string("idx too big"));

      val = value[request->idx()];
      response->set_str_arg(std::move(val.dump()));
      break;

    default:
      return grpc::Status::CANCELLED;
      break;
  }

  return grpc::Status::OK;
}

grpc::Status broker_impl::GetEndpointStats(grpc::ServerContext* context,
                                           const GenericNameOrIndex* request,
                                           GenericString* response) {
  std::vector<json11::Json::object> value;
  try {
    if (!stats::get_endpoint_stats(value))
      return grpc::Status(grpc::UNAVAILABLE, grpc::string("endpoint locked"));
  } catch (...) {
    return grpc::Status(grpc::ABORTED, grpc::string("endpoint throw error"));
  }

  bool found{false};
  json11::Json val;
  json11::Json::object object;

  switch (request->nameOrIndex_case()) {
    case GenericNameOrIndex::NAMEORINDEX_NOT_SET:
      for (auto& obj : value) {
        object["module" + obj["name"].string_value()] = obj;
      }
      val = object;
      response->set_str_arg(std::move(val.dump()));
      break;

    case GenericNameOrIndex::kStr:
      for (auto& obj : value) {
        if (obj["name"].string_value() == request->str()) {
          found = true;
          val = obj;
          response->set_str_arg(std::move(val.dump()));
          break;
        }
      }
      if (!found)
        return grpc::Status(grpc::INVALID_ARGUMENT,
                            grpc::string("name not found"));
      break;

    case GenericNameOrIndex::kIdx:

      if ((request->idx() + 1) > value.size())
        return grpc::Status(grpc::INVALID_ARGUMENT,
                            grpc::string("idx too big"));

      val = value[request->idx()];
      response->set_str_arg(std::move(val.dump()));
      break;

    default:
      return grpc::Status::CANCELLED;
      break;
  }
  return grpc::Status::OK;
}

grpc::Status broker_impl::GetGenericStats(
    grpc::ServerContext* context,
    const ::google::protobuf::Empty* request,
    GenericString* response) {
  json11::Json::object object;
  stats::get_generic_stats(object);

  json11::Json val;
  val = object;
  response->set_str_arg(std::move(val.dump()));
  return grpc::Status::OK;
}

grpc::Status broker_impl::GetSqlStats(grpc::ServerContext* context,
                                      const ::google::protobuf::Empty* request,
                                      GenericString* response) {
  json11::Json::object object;
  stats::get_mysql_stats(object);

  json11::Json val;
  val = object;
  response->set_str_arg(std::move(val.dump()));
  return grpc::Status::OK;
}
