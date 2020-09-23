/*
** Copyright 2011-2015,2017 Centreon
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

#include "com/centreon/broker/storage/factory.hh"

#include <cstring>
#include <memory>

#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/connector.hh"

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
static std::string const& find_param(config::endpoint const& cfg,
                                     std::string const& key) {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it)
    throw exceptions::msg() << "storage: no '" << key
                            << "' defined for endpoint '" << cfg.name << "'";
  return it->second;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Check if a configuration match the storage layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the configuration matches the storage layer.
 */
bool factory::has_endpoint(config::endpoint& cfg, flag* flag) const {
  if (flag)
    *flag = no;
  bool is_storage(!strncasecmp(cfg.type.c_str(), "storage", 8));
  return is_storage;
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
  (void)cache;

  // Find RRD length.
  uint32_t rrd_length;
  try {
    rrd_length = static_cast<uint32_t>(std::stoul(find_param(cfg, "length")));
  } catch (std::exception const& e) {
    rrd_length = 15552000;
    logging::error(logging::high) << "storage: the length field should contain "
                                     "a string containing a number. We use the "
                                     "default value in replacement 15552000.";
  }

  // Find interval length if set.
  uint32_t interval_length{0};
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("interval")};
    if (it != cfg.params.end()) {
      try {
        interval_length = std::stoul(it->second);
      } catch (std::exception const& e) {
        interval_length = 60;
        logging::error(logging::high) << "storage: the interval field should "
                                         "contain a string containing a "
                                         "number. We use the default value in "
                                         "replacement 60.";
      }
    }
    if (!interval_length)
      interval_length = 60;
  }

  // Find storage DB parameters.
  database_config dbcfg(cfg);

  // Rebuild check interval.
  uint32_t rebuild_check_interval(0);
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("rebuild_check_interval")};
    if (it != cfg.params.end()) {
      try {
        rebuild_check_interval = std::stoul(it->second);
      } catch (std::exception const& e) {
        rebuild_check_interval = 300;
        logging::error(logging::high)
            << "storage: the rebuild_check_interval field should "
               "contain a string containing a number. We use the default value "
               "in "
               "replacement 300.";
      }
    } else
      rebuild_check_interval = 300;
  }

  // Store or not in data_bin.
  bool store_in_data_bin(true);
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("store_in_data_bin")};
    if (it != cfg.params.end())
      store_in_data_bin = config::parser::parse_boolean(it->second);
  }

  // Connector.
  std::unique_ptr<storage::connector> c(new storage::connector);
  c->connect_to(dbcfg, rrd_length, interval_length, rebuild_check_interval,
                store_in_data_bin);
  is_acceptor = false;
  return c.release();
}
