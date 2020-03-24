//
// Created by syl on 3/11/20.
//

#ifndef CENTREON_BROKER_CORE_SRC_BROKERIMPL_HH_
#define CENTREON_BROKER_CORE_SRC_BROKERIMPL_HH_

#include "com/centreon/broker/namespace.hh"
#include "broker.grpc.pb.h"

CCB_BEGIN()
class broker_impl final : public Broker::Service {
 private:
  std::string _broker_name;

  grpc::Status GetVersion(grpc::ServerContext* context,
                          const ::google::protobuf::Empty* /*request*/,
                          Version* response) override;

  grpc::Status DebugConfReload(grpc::ServerContext* context,
                          const GenericString* /*request*/,
                          GenericResponse* response) override;

 public:
  void set_broker_name(std::string const& s) {_broker_name = s;};
};
CCB_END()

#endif  // CENTREON_BROKER_CORE_SRC_BROKERIMPL_HH_
