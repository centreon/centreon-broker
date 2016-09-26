/*
** Copyright 2011-2016 Centreon
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
#include "com/centreon/broker/notification/connector.hh"
#include "com/centreon/broker/notification/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

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
    throw (exceptions::msg() << "notification: no '" << key
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
  bool is_notif(!cfg.type.compare("notification", Qt::CaseInsensitive));
  if (is_notif) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return (is_notif);
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       The persistent cache for this module.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
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
  QString db_name(find_param(cfg, "db_name"));

  // Check replication status ?
  bool check_replication(true);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("check_replication"));
    if (it != cfg.params.end())
      check_replication = config::parser::parse_boolean(*it);
  }

  // Connector.
  std::auto_ptr<notification::connector> c(new notification::connector(cache));
  c->connect_to(
       type,
       host,
       port,
       user,
       password,
       db_name,
       check_replication);
  is_acceptor = false;
  return (c.release());
}
