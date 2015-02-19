/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
 *  @param[in] f Object to copy.
 */
factory::factory(factory const& f) : io::factory(f) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& f) {
  io::factory::operator=(f);
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
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the endpoint should be an input.
 *  @param[in] is_output true if the endpoint should be an output.
 *
 *  @return true if the endpoint match the configuration.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  bool is_sql(!cfg.type.compare("sql", Qt::CaseInsensitive)
              && is_output);
  if (is_sql) {
    // Default transaction timeout.
    if (cfg.params.find("read_timeout") == cfg.params.end()) {
      cfg.params["read_timeout"] = "2";
      cfg.read_timeout = 2;
    }
  }
  return (is_sql);
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if the endpoint should be an input.
 *  @param[in]  is_output   true if the endpoint should be an output.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Unused.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)is_input;
  (void)is_output;
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
       check_replication,
       wse);
  is_acceptor = false;
  return (c.release());
}
