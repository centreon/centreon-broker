/*
** Copyright 2011-2013 Centreon
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
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Find a parameter in configuration.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *
 *  @return Property value.
 */
static QString const& find_param(
                        config::endpoint const& cfg,
                        QString const& key) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    throw (exceptions::msg() << "SQL: no '" << key
           << "' defined for endpoint '" << cfg.name << "'");
  return (it.value());
}

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
    // Default transaction timeout.
    QMap<QString, QString>::const_iterator
      instance_timeout_it(cfg.params.find("instance_timeout"));
    if (instance_timeout_it != cfg.params.end()) {
      if (cfg.params.find("read_timeout") == cfg.params.end()) {
        int timeout(instance_timeout_it->toInt());
        QString timeout_str;
        timeout_str.setNum(timeout);
        cfg.params["read_timeout"] = timeout_str;
        cfg.read_timeout = timeout;
      }
    }
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
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Find DB type.
  QString type(find_param(cfg, "db_type"));

  // Find DB host.
  QString host(find_param(cfg, "db_host"));

  // Find DB port.
  unsigned short port(find_param(cfg, "db_port").toUShort());

  // Find DB user.
  QString user(find_param(cfg, "db_user"));

  // Find DB password.
  QString password(find_param(cfg, "db_password"));

  // Find DB name.
  QString name(find_param(cfg, "db_name"));

  // Transaction size.
  unsigned int queries_per_transaction(0);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("queries_per_transaction"));
    if (it != cfg.params.end())
      queries_per_transaction = it.value().toUInt();
    else
      queries_per_transaction = 1000;
  }

  // Cleanup check interval.
  unsigned int cleanup_check_interval(0);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("cleanup_check_interval"));
    if (it != cfg.params.end())
      cleanup_check_interval = it.value().toUInt();
  }

  // Instance timeout
  // By default, 5 minutes.
  unsigned int instance_timeout(5 * 60);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("instance_timeout"));
    if (it != cfg.params.end())
      instance_timeout = it.value().toUInt();
  }

  // Check replication status ?
  bool check_replication(true);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("check_replication"));
    if (it != cfg.params.end())
      check_replication = config::parser::parse_boolean(*it);
  }

  // Use state events ?
  bool wse(false);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("with_state_events"));
    if (it != cfg.params.end())
      wse = config::parser::parse_boolean(*it);
  }

  // Connector.
  std::auto_ptr<sql::connector> c(new sql::connector);
  c->connect_to(
       type,
       host,
       port,
       user,
       password,
       name,
       queries_per_transaction,
       cleanup_check_interval,
       instance_timeout,
       check_replication,
       wse);
  is_acceptor = false;
  return (c.release());
}
