/*
** Copyright 2011-2013,2015 Centreon
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
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/sql/connector.hh"
#include "com/centreon/broker/sql/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

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
 *  @return True if the endpoint match the configuration.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_sql(!cfg.type.compare("sql", Qt::CaseInsensitive));
  if (is_sql) {
    cfg.params["read_timeout"] = 1;
    cfg.read_timeout = 1;
  }
  return (is_sql);
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Unused.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Database configuration.
  database_config dbcfg(cfg);

  // Cleanup check interval.
  unsigned int cleanup_check_interval(0);
  {
    std::map<std::string, std::string>::const_iterator
      it{cfg.params.find("cleanup_check_interval")};
    if (it != cfg.params.end())
      cleanup_check_interval = std::stoul(it->second);
  }

  // Instance timeout
  // By default, 5 minutes.
  unsigned int instance_timeout(5 * 60);
  {
    std::map<std::string, std::string>::const_iterator
      it(cfg.params.find("instance_timeout"));
    if (it != cfg.params.end())
      instance_timeout = std::stoul(it->second);
  }

  // Use state events ?
  bool wse(false);
  {
    std::map<std::string, std::string>::const_iterator
      it(cfg.params.find("with_state_events"));
    if (it != cfg.params.end())
      wse = config::parser::parse_boolean(it->second);
  }

  // Connector.
  std::unique_ptr<sql::connector> c{new sql::connector};
  c->connect_to(
       dbcfg,
       cleanup_check_interval,
       instance_timeout,
       wse);
  is_acceptor = false;
  return c.release();
}
