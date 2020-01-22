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

#include "com/centreon/broker/rrd/factory.hh"
#include <memory>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/rrd/connector.hh"

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
static std::string find_param(config::endpoint const& cfg,
                              std::string const& key,
                              bool thrw = true,
                              std::string const& def = "") {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it) {
    if (thrw)
      throw exceptions::msg() << "RRD: no '" << key
                              << "' defined "
                                 " for endpoint '"
                              << cfg.name << "'";
    else
      return def;
  }
  return it->second;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

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
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Local socket path.
  std::string path{find_param(cfg, "path", false)};

  // Network connection.
  unsigned short port{0};
  {
    try {
      port = static_cast<uint16_t>(
          std::stoul(find_param(cfg, "port", false, "0")));
    } catch (...) {
      throw exceptions::msg() << "RRD: bad port"
                              << " defined "
                                 " for endpoint '"
                              << cfg.name << "'";
    }
  }

  // Get rrd creator cache size.
  uint32_t cache_size(16);
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("cache_size")};
    if (it != cfg.params.end())
      try {
        cache_size = std::stoul(it->second);
      } catch (std::exception const& e) {
        throw exceptions::msg() << "RRD: bad port"
                                << " defined "
                                   " for endpoint '"
                                << cfg.name << "'";
      }
  }

  // Should metrics be written ?
  bool write_metrics;
  {
    std::map<std::string, std::string>::const_iterator it(
        cfg.params.find("write_metrics"));
    if (it != cfg.params.end())
      write_metrics = config::parser::parse_boolean(it->second);
    else
      write_metrics = true;
  }

  // Should status be written ?
  bool write_status;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("write_status")};
    if (it != cfg.params.end())
      write_status = config::parser::parse_boolean(it->second);
    else
      write_status = true;
  }

  // Get metrics RRD path.
  std::string metrics_path{write_metrics ? find_param(cfg, "metrics_path")
                                         : ""};

  // Get status RRD path.
  std::string status_path{write_status ? find_param(cfg, "status_path") : ""};

  // Ignore update errors (2.4.0-compatible behavior).
  bool ignore_update_errors;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("ignore_update_errors")};
    if (it != cfg.params.end())
      ignore_update_errors = config::parser::parse_boolean(it->second);
    else
      ignore_update_errors = true;
  }

  // Create endpoint.
  std::unique_ptr<rrd::connector> endp{new rrd::connector};
  if (write_metrics)
    endp->set_metrics_path(metrics_path);
  if (write_status)
    endp->set_status_path(status_path);
  if (!path.empty())
    endp->set_cached_local(path);
  else if (port)
    endp->set_cached_net(port);
  endp->set_cache_size(cache_size);
  endp->set_write_metrics(write_metrics);
  endp->set_write_status(write_status);
  endp->set_ignore_update_errors(ignore_update_errors);
  is_acceptor = false;
  return endp.release();
}
