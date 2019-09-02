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

#include <cstring>
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
static std::string const& find_param(
                        config::endpoint const& cfg,
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
  return *this;
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this factory.
 */
io::factory* factory::clone() const {
  return new factory(*this);
}

/**
 *  Check if a configuration match the storage layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the configuration matches the storage layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_storage(!strncasecmp(cfg.type.c_str(), "storage", 8));
  if (is_storage) {
    cfg.params["read_timeout"] = "1";
    cfg.read_timeout = 1;
  }
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
  unsigned int rrd_length{static_cast<unsigned int>(std::stoul(find_param(cfg, "length")))};

  // Find interval length if set.
  unsigned int interval_length{0};
  {
    std::map<std::string, std::string>::const_iterator
      it{cfg.params.find("interval")};
    if (it != cfg.params.end())
      interval_length = std::stoul(it->second);
    if (!interval_length)
      interval_length = 60;
  }

  // Find storage DB parameters.
  database_config db_cfg(cfg);

  // Rebuild check interval.
  unsigned int rebuild_check_interval(0);
  {
    std::map<std::string, std::string>::const_iterator
      it{cfg.params.find("rebuild_check_interval")};
    if (it != cfg.params.end())
      rebuild_check_interval = std::stoul(it->second);
    else
      rebuild_check_interval = 300;
  }

  // Store or not in data_bin.
  bool store_in_data_bin(true);
  {
    std::map<std::string, std::string>::const_iterator
      it{cfg.params.find("store_in_data_bin")};
    if (it != cfg.params.end())
      store_in_data_bin = config::parser::parse_boolean(it->second);
  }

  // Insert entries or not in index_data.
  bool insert_in_index_data(false);
  {
    std::map<std::string, std::string>::const_iterator
      it{cfg.params.find("insert_in_index_data")};
    if (it != cfg.params.end())
      insert_in_index_data = config::parser::parse_boolean(it->second);
  }

  // Connector.
  std::unique_ptr<storage::connector> c(new storage::connector);
  c->connect_to(
       db_cfg,
       rrd_length,
       interval_length,
       rebuild_check_interval,
       store_in_data_bin,
       insert_in_index_data);
  is_acceptor = false;
  return c.release();
}
