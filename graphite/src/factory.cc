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
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/graphite//connector.hh"
#include "com/centreon/broker/graphite/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

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
    throw (exceptions::msg() << "graphite: no '" << key
           << "' defined for endpoint '" << cfg.name << "'");
  return (it.value().toStdString());
}

/**
 *  Get a parameter in configuration, or return a default value.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *  @param[in] def The default value if nothing found.
 *
 *  @return Property value.
 */
static std::string get_string_param(
              config::endpoint const& cfg,
              QString const& key,
              std::string const& def) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    return (def);
  else
    return (it->toStdString());
}

/**
 *  Get a parameter in configuration, or return a default value.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *  @param[in] def The default value if nothing found.
 *
 *  @return Property value.
 */
static unsigned int get_uint_param(
             config::endpoint const& cfg,
             QString const& key,
             unsigned int def) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    return (def);
  else
    return (it->toUInt());
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
  bool is_ifdb(!cfg.type.compare("graphite", Qt::CaseInsensitive)
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
 *  @param[in]  cache       The persistent cache.
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

  std::string db_host(find_param(cfg, "db_host"));

  std::string metric_naming(
        get_string_param(cfg, "metric_naming", "centreon.metrics.$METRICID$"));
  std::string status_naming(
        get_string_param(cfg, "status_naming", "centreon.statuses.$INDEXID$"));
  unsigned short db_port(
        get_uint_param(cfg, "db_port", 80));
  std::string db_user(
        get_string_param(cfg, "db_user", ""));
  std::string db_password(
        get_string_param(cfg, "db_password", ""));
  unsigned int queries_per_transaction(
                 get_uint_param(cfg, "queries_per_transaction", 1));

  // Connector.
  std::auto_ptr<graphite::connector> c(new graphite::connector);
  c->connect_to(
       metric_naming,
       status_naming,
       db_user,
       db_password,
       db_host,
       db_port,
       queries_per_transaction);
  is_acceptor = false;
  return (c.release());
}
