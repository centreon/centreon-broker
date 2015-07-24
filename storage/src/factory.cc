/*
** Copyright 2011-2015 Merethis
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
#include "com/centreon/broker/storage/connector.hh"
#include "com/centreon/broker/storage/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Static Objects            *
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
    throw (exceptions::msg() << "storage: no '" << key
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
 *  Clone this object.
 *
 *  @return Exact copy of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration match the storage layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the configuration matches the storage layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_storage(!cfg.type.compare("storage", Qt::CaseInsensitive));
  if (is_storage) {
    // Transaction timeout.
    if (cfg.params.find("read_timeout") == cfg.params.end()) {
      cfg.params["read_timeout"] = "2";
      cfg.read_timeout = 2;
    }
  }
  return (is_storage);
}

/**
 *  Build a storage endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching the given configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Find lengths.
  unsigned int rrd_length(find_param(cfg, "length").toUInt());

  // Find storage DB parameters.
  database_config db_cfg;
  db_cfg.set_type(find_param(cfg, "db_type").toStdString());
  db_cfg.set_host(find_param(cfg, "db_host").toStdString());
  db_cfg.set_port(find_param(cfg, "db_port").toUShort());
  db_cfg.set_user(find_param(cfg, "db_user").toStdString());
  db_cfg.set_password(find_param(cfg, "db_password").toStdString());
  db_cfg.set_name(find_param(cfg, "db_name").toStdString());

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
  db_cfg.set_queries_per_transaction(queries_per_transaction);

  // Rebuild check interval.
  unsigned int rebuild_check_interval(0);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("rebuild_check_interval"));
    if (it != cfg.params.end())
      rebuild_check_interval = it.value().toUInt();
    else
      rebuild_check_interval = 300;
  }

  // Check replication status ?
  bool check_replication(true);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("check_replication"));
    if (it != cfg.params.end())
      check_replication = config::parser::parse_boolean(*it);
  }
  db_cfg.set_check_replication(check_replication);

  // Store or not in data_bin.
  bool store_in_data_bin(true);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("store_in_data_bin"));
    if (it != cfg.params.end())
      store_in_data_bin = config::parser::parse_boolean(*it);
  }

  // Insert entries or not in index_data.
  bool insert_in_index_data(false);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("insert_in_index_data"));
    if (it != cfg.params.end())
      insert_in_index_data = config::parser::parse_boolean(*it);
  }

  // Connector.
  std::auto_ptr<storage::connector> c(new storage::connector);
  c->connect_to(
       db_cfg,
       rrd_length,
       rebuild_check_interval,
       store_in_data_bin,
       insert_in_index_data);
  is_acceptor = false;
  return (c.release());
}
