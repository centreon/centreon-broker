/*
** Copyright 2014-2015 Centreon
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
#include "com/centreon/broker/bam/connector.hh"
#include "com/centreon/broker/bam/factory.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

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
 *  Check if a configuration match the BAM layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if the configuration matches the BAM layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_bam(!cfg.type.compare("bam", Qt::CaseInsensitive));
  bool is_bam_bi(!cfg.type.compare("bam_bi", Qt::CaseInsensitive));
  if (is_bam || is_bam_bi) {
    cfg.params["read_timeout"] = "1";
    cfg.read_timeout = 1;
  }
  return (is_bam || is_bam_bi);
}

/**
 *  Build a BAM endpoint from a configuration.
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

  // Find DB parameters.
  database_config db_cfg(cfg);

  // Is it a BAM or BAM-BI output ?
  bool is_bam_bi(!cfg.type.compare("bam_bi", Qt::CaseInsensitive));

  // Storage database name.
  std::string storage_db_name;
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("storage_db_name"));
    if (it != cfg.params.end())
      storage_db_name = it->toStdString();
  }

  // Connector.
  std::auto_ptr<bam::connector> c(new bam::connector);
  if (is_bam_bi)
    c->connect_reporting(db_cfg);
  else
    c->connect_monitoring(db_cfg, storage_db_name);
  is_acceptor = false;
  return (c.release());
}
