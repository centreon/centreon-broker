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

#include "com/centreon/broker/compression/factory.hh"

#include <cstring>
#include <memory>

#include "com/centreon/broker/compression/opener.hh"
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/parser.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Check if an endpoint configuration match the compression layer.
 *
 *  @param[in] cfg  Configuration object.
 *  @param[out] flag Returns no, maybe or yes, corresponding to the no, auto,
 *                   yes configured in the configuration file.
 *
 *  @return False everytime because the compression layer must not be set at
 *  the broker configuration. This avoids the compression while the negotiation
 *  is running. We will be able to add this endpoint later, following the flag
 *  value.
 */
bool factory::has_endpoint(config::endpoint& cfg, flag* flag) const {
  if (flag) {
    auto it = cfg.params.find("compression");
    if (it == cfg.params.end() || strncasecmp(it->second.c_str(), "no", 3) == 0)
      *flag = no;
    else if (strncasecmp(it->second.c_str(), "auto", 5) == 0)
      *flag = maybe;
    else if (strncasecmp(it->second.c_str(), "yes", 4) == 0)
      *flag = yes;
  }
  return false;
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
  int level{-1};
  std::map<std::string, std::string>::const_iterator it{
      cfg.params.find("compression_level")};
  if (it != cfg.params.end())
    level = std::stol(it->second);

  // Get buffer size.
  uint32_t size(0);
  it = cfg.params.find("compression_buffer");
  if (it != cfg.params.end())
    size = std::stoul(it->second);

  // Create compression object.
  std::unique_ptr<compression::opener> openr(new compression::opener);
  openr->set_level(level);
  openr->set_size(size);
  return openr.release();
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
std::shared_ptr<io::stream> factory::new_stream(std::shared_ptr<io::stream> to,
                                                bool is_acceptor,
                                                std::string const& proto_name) {
  (void)is_acceptor;
  (void)proto_name;
  std::shared_ptr<io::stream> s{std::make_shared<stream>()};
  s->set_substream(to);
  return s;
}
