/*
** Copyright 2011-2012 Centreon
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

#include <cstdlib>
#include <memory>
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker::io;

// Class instance.
static protocols* gl_protocols = NULL;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
protocols::~protocols() {
  logging::info(logging::low) << "protocols: destruction ("
    << _protocols.size() << " protocols still registered)";
}

/**
 *  Get an iterator to the first registered protocol.
 *
 *  @return Iterator to the beginning of the protocol list.
 */
QMap<QString, protocols::protocol>::const_iterator protocols::begin() const {
  return (_protocols.begin());
}

/**
 *  Get an iterator to the end of the protocol list.
 *
 *  @return Iterator to the end of the protocol list.
 */
QMap<QString, protocols::protocol>::const_iterator protocols::end() const {
  return (_protocols.end());
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
protocols& protocols::instance() {
  return (*gl_protocols);
}

/**
 *  Load singleton.
 */
void protocols::load() {
  if (!gl_protocols)
    gl_protocols = new protocols;
  return ;
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
void protocols::reg(
                  QString const& name,
                  factory const& fac,
                  unsigned short osi_from,
                  unsigned short osi_to) {
  // Set protocol structure.
  protocol p;
  p.endpntfactry = misc::shared_ptr<factory>(fac.clone());
  p.osi_from = osi_from;
  p.osi_to = osi_to;

  // Register protocol in protocol list.
  _protocols[name] = p;

  return ;
}

/**
 *  Unload the singleton.
 */
void protocols::unload() {
  delete gl_protocols;
  gl_protocols = NULL;
  return ;
}

/**
 *  Unregister a protocol.
 *
 *  @param[in] name Protocol name.
 */
void protocols::unreg(QString const& name) {
  _protocols.remove(name);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
protocols::protocols() {}
