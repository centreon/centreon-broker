/*
** Copyright 2011-2017 Centreon
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

#include <json11.hpp>
#include <memory>
#include <sstream>
#include <vector>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/connector.hh"
#include "com/centreon/broker/influxdb/factory.hh"
#include "com/centreon/broker/influxdb/column.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;
using namespace json11;

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
static std::string find_param(
                     config::endpoint const& cfg,
                     QString const& key) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    throw (exceptions::msg() << "influxdb: no '" << key
           << "' defined for endpoint '" << cfg.name << "'");
  return (it.value().toStdString());
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
 *  @return True if the configuration matches the storage layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_ifdb(!cfg.type.compare("influxdb", Qt::CaseInsensitive));
  if (is_ifdb) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return (is_ifdb);
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
                         std::shared_ptr<persistent_cache> cache) const {
  std::string user(find_param(cfg, "db_user"));
  std::string passwd(find_param(cfg, "db_password"));
  std::string addr(find_param(cfg, "db_host"));
  std::string db(find_param(cfg, "db_name"));

  unsigned short port(0);
  {
    std::stringstream ss;
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("db_port"));
    if (it == cfg.params.end())
      port = 8086;
    else {
      ss << it->toStdString();
      ss >> port;
      if (!ss.eof())
        throw (exceptions::msg() << "influxdb: couldn't parse port '" << ss.str()
               << "' defined for endpoint '" << cfg.name << "'");
    }
  }

  unsigned int queries_per_transaction(0);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("queries_per_transaction"));
    if (it != cfg.params.end())
      queries_per_transaction = it.value().toUInt();
    else
      queries_per_transaction = 1000;
  }

  // Get status query.
  std::string status_timeseries(find_param(cfg, "status_timeseries"));
  std::vector<column> status_column_list;
  Json const& status_columns = cfg.cfg["status_column"];
  for (Json const& object : status_columns.array_items()) {
    Json const& name{object["name"]};
    Json const& value{object["value"]};
    Json const& is_tag{object["is_tag"]};
    Json const& type{object["type"]};
    if (name.is_null() || !name.is_string() ||
        name.string_value().empty() || !value.is_string() ||
        value.string_value().empty())
      throw (exceptions::msg())
             << "influxdb: couldn't get the configuration of a status column";
    status_column_list.push_back(column(
      name.string_value(),
      value.string_value(),
      is_tag.bool_value(),
      column::parse_type(type.string_value())));
  }

  // Get metric query.*/
  std::string metric_timeseries(find_param(cfg, "metrics_timeseries"));
  std::vector<column> metric_column_list;
  Json const& metric_columns = cfg.cfg["metrics_column"];
  for (Json const& object : metric_columns.array_items()) {
    Json const& name{object["name"]};
    Json const& value{object["value"]};
    Json const& is_tag{object["is_tag"]};
    Json const& type{object["type"]};
    if (name.is_null() || !name.is_string() ||
        name.string_value().empty() || !value.is_string() ||
        value.string_value().empty())
      throw (exceptions::msg())
             << "influxdb: couldn't get the configuration of a metric column";
    metric_column_list.push_back(column(
      name.string_value(),
      value.string_value(),
      is_tag.bool_value(),
      column::parse_type(type.string_value())));
  }

  // Connector.
  std::unique_ptr<influxdb::connector> c(new influxdb::connector);
  c->connect_to(
       user,
       passwd,
       addr,
       port,
       db,
       queries_per_transaction,
       status_timeseries,
       status_column_list,
       metric_timeseries,
       metric_column_list,
       cache);
  is_acceptor = false;
  return (c.release());
}
