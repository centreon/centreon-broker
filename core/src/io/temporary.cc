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
