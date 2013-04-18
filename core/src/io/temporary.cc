/*
** Copyright 2011-2013 Merethis
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

#include <QMutexLocker>
#include "com/centreon/broker/io/temporary.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

static temporary* gl_instance = NULL;

/**
 *  Create new temporary.
 *
 *  @param[in] name Sufix to add on the temporary path.
 *
 *  @return The new temporary endpoint.
 */
misc::shared_ptr<io::stream> temporary::create(QString const& name) {
  QMutexLocker guard(&_lock);
  if (!_endp)
    return (NULL);
  return (_endp->open(name));
}

/**
 *  Get singleton instance.
 */
temporary& temporary::instance() {
  return (*gl_instance);
}

/**
 *  Load singleton.
 */
void temporary::load() {
  if (!gl_instance)
    gl_instance = new temporary;
}

/**
 *  Set the temporary endpoint.
 *
 *  @param[in] endp The temporary endpoint.
 */
void temporary::set(misc::shared_ptr<io::endpoint> endp) {
  QMutexLocker guard(&_lock);
  _endp = endp;
}

/**
 *  Unload singleton.
 */
void temporary::unload() {
  delete gl_instance;
  gl_instance = NULL;
}

/**
 *  Default constructor.
 */
temporary::temporary() {

}

/**
 *  Destructor.
 */
temporary::~temporary() {

}
