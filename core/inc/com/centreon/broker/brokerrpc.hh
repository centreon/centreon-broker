//
// Created by syl on 3/11/20.
//

#ifndef CENTREON_BROKER_CORE_SRC_BROKERRPC_HH_
#define CENTREON_BROKER_CORE_SRC_BROKERRPC_HH_


#include <string>
#include <memory>
#include <grpcpp/server.h>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/broker_impl.hh"

CCB_BEGIN()
class brokerrpc final {
  std::unique_ptr<grpc::Server> _server;
 public:
  brokerrpc(const std::string& address, uint16_t port, std::string const& broker_name);
  brokerrpc() = delete;
  brokerrpc(const brokerrpc&) = delete;
  ~brokerrpc() = default;
  void shutdown();
};

CCB_END()

#endif  // CENTREON_BROKER_CORE_SRC_BROKERRPC_HH_
