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
#include "com/centreon/broker/version.hh"
#include "com/centreon/broker/log_v2.hh"

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
                               BrokerVersion* response) {
  response->set_major(major);
  response->set_minor(minor);
  response->set_patch(patch);
  return grpc::Status::OK;
}
grpc::Status broker_impl::DebugConfReload(grpc::ServerContext* context,
                                     const BrokerGenericString* request,
                                     BrokerGenericResponse* response) {
  std::string err;
  if(log_v2::instance().load(request->str_arg(), _broker_name, err)) {
    response->set_ok(true);
    response->set_err_msg("");
  } else {
    response->set_ok(false);
    response->set_err_msg(std::move(err));
  }

  return grpc::Status::OK;
}
