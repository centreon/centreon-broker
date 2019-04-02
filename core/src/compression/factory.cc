/*
** Copyright 2011-2013 Centreon
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
#include "com/centreon/broker/compression/factory.hh"
#include "com/centreon/broker/compression/opener.hh"
#include "com/centreon/broker/compression/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

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
 *  @return A copy of this object.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint configuration match the compression layer.
 *
 *  @param[in] cfg  Configuration object.
 *
 *  @return True if the configuration matches the compression layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("compression"));
  return ((cfg.params.end() != it)
          && it->compare("auto", Qt::CaseInsensitive)
          && config::parser::parse_boolean(*it));
}

/**
 *  Check if endpoint configuration do not match the compression layer.
 *
 *  @param[in] cfg  Configuration object.
 *
 *  @return True if the configuration does not match the compression
 *          layer.
 */
bool factory::has_not_endpoint(config::endpoint& cfg) const {
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("compression"));
  return (((it != cfg.params.end())
           && it->compare("auto", Qt::CaseInsensitive))
          ? !has_endpoint(cfg)
          : false);
}

/**
 *  Create an endpoint matching the configuration object.
 *
 *  @param[in]  cfg         Configuration object.
 *  @param[out] is_acceptor Unused.
 *  @param[in]  cache       cache
 *
 *  @return New endpoint object.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  (void)is_acceptor;
  (void)cache;

  // Get compression level.
  int level(-1);
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("compression_level"));
  if (it != cfg.params.end())
    level = it.value().toInt();

  // Get buffer size.
  unsigned int size(0);
  it = cfg.params.find("compression_buffer");
  if (it != cfg.params.end())
    size = it.value().toUInt();

  // Create compression object.
  std::unique_ptr<compression::opener> openr(new compression::opener);
  openr->set_level(level);
  openr->set_size(size);
  return (openr.release());
}

/**
 *  Create a new compression stream.
 *
 *  @param[in] to          Lower-layer stream.
 *  @param[in] is_acceptor Unused.
 *  @param[in] proto_name  Unused.
 *
 *  @return New compression stream.
 */
misc::shared_ptr<io::stream> factory::new_stream(
                                        misc::shared_ptr<io::stream> to,
                                        bool is_acceptor,
                                        QString const& proto_name) {
  (void)is_acceptor;
  (void)proto_name;
  misc::shared_ptr<io::stream> s(new stream);
  s->set_substream(to);
  return (s);
}
