/*
** Copyright 2011-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cassert>
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

/**
 *  @brief Copy constructor.
 *
 *  Should not be used. Any attempt to use this constructor will result
 *  in a call to abort().
 *
 *  @param[in] p Unused.
 */
protocols::protocols(protocols const& p) {
  (void)p;
  assert(!"protocols are not copyable");
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Should not be used. Any attempt to use this method will result in a
 *  call to abort().
 *
 *  @param[in] p Unused.
 *
 *  @return This object.
 */
protocols& protocols::operator=(protocols const& p) {
  (void)p;
  assert(!"protocols are not copyable");
  abort();
  return (*this);
}
