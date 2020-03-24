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
                               Version* response) {
  response->set_major(major);
  response->set_minor(minor);
  response->set_patch(patch);
  return grpc::Status::OK;
}
grpc::Status broker_impl::ConfReload(grpc::ServerContext* context,
                                     const GenericString* request,
                                     GenericResponse* response) {
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
