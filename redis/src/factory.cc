/*
** Copyright 2018 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/redis/connector.hh"
#include "com/centreon/broker/redis/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
factory::factory(factory const& other) : io::factory(other) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& other) {
  io::factory::operator=(other);
  return (*this);
}

/**
 *  Clone the factory.
 *
 *  @return Copy of the factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint match a configuration.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the endpoint match the configuration.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_redis(!cfg.type.compare("redis", Qt::CaseInsensitive));
  if (is_redis) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_redis;
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       The persistent cache.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  std::string address;
  unsigned short port;
  unsigned int queries_per_transaction;
  std::string password;

  // Redis server IP address.
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("host"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "redis: no 'host' defined "
             << "for endpoint '" << cfg.name << "'");
    address = it.value().toStdString();
  }

  // Redis server port.
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("port"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "redis: no 'port' defined "
             << "for endpoint '" << cfg.name << "'");
    port = it.value().toUInt();
  }

  // Redis server password.
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("password"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "redis: no 'password' defined "
             << "for endpoint '" << cfg.name << "'");
    password = it.value().toStdString();
  }

  // Queries per transaction.
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("queries_per_transaction"));
    if (it == cfg.params.end())
      queries_per_transaction = 1;
    else
      queries_per_transaction = it.value().toUInt();
  }

  // Connector.
  std::auto_ptr<redis::connector> c(new redis::connector);
  c->connect_to(address, port, password, queries_per_transaction);
  is_acceptor = false;
  return c.release();
}
