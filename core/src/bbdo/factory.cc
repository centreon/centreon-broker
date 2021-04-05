/*
** Copyright 2013,2015,2017 Centreon
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

#include "com/centreon/broker/bbdo/factory.hh"

#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/connector.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  @brief Check if a configuration supports this protocol.
 *
 *  The endpoint 'protocol' tag must have the 'bbdo' value.
 *
 *  @param[in] cfg       Object configuration.
 *
 *  @return True if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint& cfg, flag* flag) {
  std::map<std::string, std::string>::const_iterator it{
      cfg.params.find("protocol")};
  if (flag)
    *flag = no;
  return it != cfg.params.end() && it->second == "bbdo";
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Return value.
  io::endpoint* retval = nullptr;

  // Coarse endpoint ?
  bool coarse = false;
  {
    auto it = cfg.params.find("coarse");
    if (it != cfg.params.end())
      coarse = config::parser::parse_boolean(it->second);
  }

  // Negotiation allowed ?
  bool negotiate = false;
  std::pair<std::string, std::string> extensions;
  if (!coarse) {
    std::map<std::string, std::string>::const_iterator it(
        cfg.params.find("negotiation"));
    if (it == cfg.params.end() || it->second != "no")
      negotiate = true;
    extensions = _extensions(cfg);
  }

  // Ack limit.
  uint32_t ack_limit{1000};
  {
    std::map<std::string, std::string>::const_iterator it(
        cfg.params.find("ack_limit"));
    if (it != cfg.params.end())
      try {
        ack_limit = std::stoul(it->second);
      } catch (const std::exception& e) {
        logging::config(logging::high)
            << "BBDO: Bad value for ack_limit, it must be an integer.";
      }
  }

  // Create object.
  std::string host;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("host")};
    if (it != cfg.params.end())
      host = it->second;
  }

  if (is_acceptor) {
    // This flag is just needed to know if we keep the retention or not...
    // When the connection is made to the map server, no retention is needed,
    // otherwise we want it.
    bool keep_retention{false};
    auto it = cfg.params.find("one_peer_retention_mode");
    if (it != cfg.params.end())
      keep_retention = config::parser::parse_boolean(it->second);

    // One peer retention mode? (i.e. keep_retention + acceptor_is_output)
    bool acceptor_is_output = cfg.get_io_type() == config::endpoint::output;
    if (!acceptor_is_output && keep_retention)
      log_v2::bbdo()->error(
          "BBDO: Configuration error, the one peer retention mode should be "
          "set only when the connection is reversed");

    retval =
        new bbdo::acceptor(cfg.name, negotiate, extensions, cfg.read_timeout,
                           acceptor_is_output, coarse, ack_limit);
    if (acceptor_is_output && keep_retention)
      is_acceptor = false;
    log_v2::bbdo()->debug("BBDO: new acceptor {}", cfg.name);
  } else {
    bool connector_is_input = cfg.get_io_type() == config::endpoint::input;
    retval = new bbdo::connector(negotiate, extensions, cfg.read_timeout,
                                 connector_is_input, coarse, ack_limit);
    log_v2::bbdo()->debug("BBDO: new connector {}", cfg.name);
  }
  return retval;
}

/**
 *  Get available extensions for an endpoint. Two strings are returned:
 *  * the first one contains all the extensions supported by the endpoint.
 *  * the second one contains the mandatory extensions. This one is needed
 *    to report errors.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  return a pair of two strings, extensions and mandatories.
 */
std::pair<std::string, std::string> factory::_extensions(
    config::endpoint& cfg) const {
  std::string extensions;
  std::string mandatory;
  for (std::map<std::string, io::protocols::protocol>::const_iterator
           it{io::protocols::instance().begin()},
       end{io::protocols::instance().end()};
       it != end; ++it) {
    flag flag;
    bool has = it->second.endpntfactry->has_endpoint(cfg, &flag);
    if (it->second.osi_from > 1 && it->second.osi_to < 7 &&
        (has || flag != io::factory::flag::no)) {
      if (!extensions.empty())
        extensions.append(" ");
      extensions.append(it->first);
      if (flag == yes) {
        if (!mandatory.empty())
          mandatory.append(" ");
        mandatory.append(it->first);
      }
    }
  }
  return std::make_pair(extensions, mandatory);
}
