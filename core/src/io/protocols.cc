/*
** Copyright 2011-2012, 2021 Centreon
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

#include "com/centreon/broker/io/protocols.hh"
#include <cassert>
#include <cstdlib>
#include <memory>
#include "com/centreon/broker/compression/factory.hh"
#include "com/centreon/broker/file/factory.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker::io;

// Class instance.
static protocols* gl_protocols = nullptr;

/**
 *  Get an iterator to the first registered protocol.
 *
 *  @return Iterator to the beginning of the protocol list.
 */
std::map<std::string, protocols::protocol>::const_iterator protocols::begin()
    const {
  return _protocols.begin();
}

/**
 *  Get an iterator to the end of the protocol list.
 *
 *  @return Iterator to the end of the protocol list.
 */
std::map<std::string, protocols::protocol>::const_iterator protocols::end()
    const {
  return _protocols.end();
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
protocols& protocols::instance() {
  assert(gl_protocols);
  return *gl_protocols;
}

/**
 *  Load singleton.
 */
void protocols::load() {
  if (!gl_protocols)
    gl_protocols = new protocols;
}

/**
 *  Register a protocol.
 *
 *  @param[in] name         Protocol name.
 *  @param[in] endpntfactry Factory that allocates new endpoint of the
 *                          matching protocol type.
 *  @param[in] osi_from     OSI layer this protocol supports.
 *  @param[in] osi_to       OSI layer this protocol supports.
 */
void protocols::reg(std::string const& name,
                    std::shared_ptr<factory> fac,
                    unsigned short osi_from,
                    unsigned short osi_to) {
  // Set protocol structure.
  protocol p;
  p.endpntfactry = fac;
  p.osi_from = osi_from;
  p.osi_to = osi_to;

  // Register protocol in protocol list.
  log_v2::core()->info("protocols: registering protocol ('{}' (layers {}-{})",
                       name, osi_from, osi_to);
  _protocols[name] = p;
}

/**
 *  Unload the singleton.
 */
void protocols::unload() {
  delete gl_protocols;
  gl_protocols = nullptr;
}

/**
 *  Unregister a protocol.
 *
 *  @param[in] name Protocol name.
 */
void protocols::unreg(std::string const& name) {
  log_v2::core()->info("protocols: unregistering protocol '{}'", name);
  _protocols.erase(name);
}

/**
 *  Default constructor.
 */
protocols::protocols() {
  // Registering internal protocols
  reg("file", std::make_shared<file::factory>(), 1, 3);
  reg("compression", std::make_shared<compression::factory>(), 6, 6);
}

/**
 * @brief Destructor.
 */
protocols::~protocols() noexcept {
  unreg("compression");
  unreg("file");
  log_v2::core()->info("protocols: destruction ({} protocols still registered)",
                       _protocols.size());
}
