/*
** Copyright 2011-2014 Merethis
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
#include <sstream>
#include <vector>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/connector.hh"
#include "com/centreon/broker/influxdb/factory.hh"
#include "com/centreon/broker/influxdb/column.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

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
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if endpoint should act as input.
 *  @param[in] is_output true if endpoint should act as output.
 *
 *  @return true if the configuration matches the storage layer.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  bool is_ifdb(!cfg.type.compare("influxdb", Qt::CaseInsensitive)
                  && is_output);
  return (is_ifdb);
}

/**
 *  Build a storage endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if endpoint should act as input.
 *  @param[in]  is_output   true if endpoint should act as output.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching the given configuration.
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

  std::string user(find_param(cfg, "db_user"));
  std::string passwd(find_param(cfg, "db_password"));
  std::string addr(find_param(cfg, "db_host"));
  std::string db(find_param(cfg, "db_name"));

  unsigned short port(0);
  {
    std::stringstream ss;
    ss << find_param(cfg, "db_port");
    ss >> port;
    if (!ss.eof())
      throw (exceptions::msg() << "influxdb: couldn't parse port '" << ss.str()
             << "' defined for endpoint '" << cfg.name << "'");
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

  std::string version;
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("influxdb_version"));
    if (it != cfg.params.end())
      version = it.value().toStdString();
    else
      version = "0.9";
  }

  // Get status query.
  std::string status_timeseries(find_param(cfg, "status_timeseries"));
  std::vector<column> status_column_list;
  QDomNodeList status_columns = cfg.cfg.elementsByTagName("status_column");
  for (size_t i = 0; i < status_columns.size(); ++i) {
    QDomNode status = status_columns.item(i);
    QDomNode name = status.namedItem("name");
    QDomNode value = status.namedItem("value");
    QDomNode is_tag = status.namedItem("is_tag");
    QDomNode type = status.namedItem("type");
    if (name.isNull() || value.isNull())
      throw (exceptions::msg())
             << "influxdb: couldn't get the configuration of a status column";
    status_column_list.push_back(column(
      name.toElement().text().toStdString(),
      value.toElement().text().toStdString(),
      is_tag.isNull() ?
        false :
        config::parser::parse_boolean(is_tag.toElement().text()),
      type.isNull() ?
        column::number :
        column::string));

  }

  // Get metric query.
  std::string metric_timeseries(find_param(cfg, "metrics_timeseries"));
  std::vector<column> metric_column_list;
  QDomNodeList metric_columns = cfg.cfg.elementsByTagName("metrics_column");
  for (size_t i = 0; i < metric_columns.size(); ++i) {
    QDomNode metric = metric_columns.item(i);
    QDomNode name = metric.namedItem("name");
    QDomNode value = metric.namedItem("value");
    QDomNode is_tag = metric.namedItem("is_tag");
    QDomNode type = metric.namedItem("type");
    if (name.isNull() || value.isNull())
      throw (exceptions::msg())
             << "influxdb: couldn't get the configuration of a metric column";
    metric_column_list.push_back(column(
      name.toElement().text().toStdString(),
      value.toElement().text().toStdString(),
      is_tag.isNull() ?
        false :
        config::parser::parse_boolean(is_tag.toElement().text()),
      type.isNull() ?
        column::number :
        column::string));
  }

  // Connector.
  std::auto_ptr<influxdb::connector> c(new influxdb::connector);
  c->connect_to(
       user,
       passwd,
       addr,
       port,
       db,
       queries_per_transaction,
       version,
       status_timeseries,
       status_column_list,
       metric_timeseries,
       metric_column_list,
       cache);
  is_acceptor = false;
  return (c.release());
}
