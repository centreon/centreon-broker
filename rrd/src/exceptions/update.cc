/*
** Copyright 2011 Centreon
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

#include "com/centreon/broker/rrd/exceptions/update.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
update::update() throw() {}

/**
 *  Copy constructor.
 *
 *  @param[in] u Object to copy.
 */
update::update(update const& u) throw() : broker::exceptions::msg(u) {}

/**
 *  Destructor.
 */
update::~update() throw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] u Object to copy.
 *
 *  @return This object.
 */

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
exceptions::msg* update::clone() const {
  return (new update(*this));
}

/**
 *  Rethrow the exception.
 */
void update::rethrow() const {
  throw(*this);
  return;
}
