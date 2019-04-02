/*
** Copyright 2011-2015 Centreon
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
#include "com/centreon/broker/rrd/connector.hh"
#include "com/centreon/broker/rrd/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Search for a property value.
 *
 *  @param[in] cfg  Configuration object.
 *  @param[in] key  Key to get.
 *  @param[in] thrw Should throw if value is not found.
 *  @param[in] def  Default value.
 */
static QString find_param(
                 config::endpoint const& cfg,
                 QString const& key,
                 bool thrw = true,
                 QString const& def = QString()) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it) {
    if (thrw)
      throw (exceptions::msg() << "RRD: no '" << key << "' defined " \
               " for endpoint '" << cfg.name << "'");
    else
      return (def);
  }
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
 *  Check if a configuration match the RRD layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if the configuration matches the RRD layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  return (cfg.type == "rrd");
}

/**
 *  Build a RRD endpoint from a configuration.
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

  // Local socket path.
  QString path(find_param(cfg, "path", false));

  // Network connection.
  unsigned short port;
  port = find_param(cfg, "port", false, "0").toUShort();

  // Get rrd creator cache size.
  unsigned int cache_size(16);
  {
    QMap<QString, QString>::iterator
      it(cfg.params.find("cache_size"));
    if (it != cfg.params.end())
      cache_size = it->toUInt();
  }

  // Should metrics be written ?
  bool write_metrics;
  {
    QMap<QString, QString>::iterator
      it(cfg.params.find("write_metrics"));
    if (it != cfg.params.end())
      write_metrics = config::parser::parse_boolean(*it);
    else
      write_metrics = true;
  }

  // Should status be written ?
  bool write_status;
  {
    QMap<QString, QString>::iterator
      it(cfg.params.find("write_status"));
    if (it != cfg.params.end())
      write_status = config::parser::parse_boolean(*it);
    else
      write_status = true;
  }

  // Get metrics RRD path.
  QString metrics_path(write_metrics
                       ? find_param(cfg, "metrics_path")
                       : "");

  // Get status RRD path.
  QString status_path(write_status
                      ? find_param(cfg, "status_path")
                      : "");

  // Ignore update errors (2.4.0-compatible behavior).
  bool ignore_update_errors;
  {
    QMap<QString, QString>::iterator
      it(cfg.params.find("ignore_update_errors"));
    if (it != cfg.params.end())
      ignore_update_errors = config::parser::parse_boolean(*it);
    else
      ignore_update_errors = true;
  }

  // Create endpoint.
  std::unique_ptr<rrd::connector> endp(new rrd::connector);
  if (write_metrics)
    endp->set_metrics_path(metrics_path);
  if (write_status)
    endp->set_status_path(status_path);
  if (!path.isEmpty())
    endp->set_cached_local(path);
  else if (port)
    endp->set_cached_net(port);
  endp->set_cache_size(cache_size);
  endp->set_write_metrics(write_metrics);
  endp->set_write_status(write_status);
  endp->set_ignore_update_errors(ignore_update_errors);
  is_acceptor = false;
  return (endp.release());
}
