/*
** Copyright 2013,2015-2016 Centreon
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
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/dumper/factory.hh"
#include "com/centreon/broker/dumper/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

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
static std::string const& find_param(
                        config::endpoint const& cfg,
                        std::string const& key) {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it)
    throw exceptions::msg() << "dumper: no '" << key
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
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this object.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration match the dumper layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches the dumper layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_dumper(cfg.type == "dumper");
  bool is_dump_fifo(cfg.type == "dump_fifo");
  bool is_dump_dir(cfg.type == "dump_dir");
  bool is_db_cfg_reader(cfg.type == "db_cfg_reader");
  bool is_db_cfg_writer(cfg.type == "db_cfg_writer");
  if (is_dump_dir) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return (is_dumper
          || is_dump_fifo
          || is_dump_dir
          || is_db_cfg_reader
          || is_db_cfg_writer);
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Persistent cache.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  // Set acceptor flag.
  is_acceptor = false;

  // Get the type of this dumper.
  opener::dumper_type type = opener::dump;
  if (cfg.type == "dump")
    type = opener::dump;
  else if (cfg.type == "dump_fifo")
    type = opener::dump_fifo;
  else if (cfg.type == "dump_dir")
    type = opener::dump_dir;
  else if (cfg.type == "db_cfg_reader")
    type = opener::db_cfg_reader;
  else if (cfg.type == "db_cfg_writer")
    type = opener::db_cfg_writer;

  // Opener that should be set.
  std::unique_ptr<opener> openr(new opener);
  openr->set_name(cfg.name);
  openr->set_type(type);
  openr->set_cache(cache);

  // DB configuration dumpers.
  if ((type == opener::db_cfg_reader)
      || (type == opener::db_cfg_writer)) {
    // DB parameters.
    database_config dbcfg(cfg);

    // Set opener properties.
    openr->set_db(dbcfg);
  }
  // Filesystem dumpers.
  else {
    // Find path to the dumper.
    std::string path{find_param(cfg, "path")};

    // Find tagname to the dumper.
    std::string tagname{find_param(cfg, "tagname")};

    // Set opener properties.
    openr->set_path(path);
    openr->set_tagname(tagname);
  }

  return openr.release();
}
