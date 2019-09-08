/*
** Copyright 2009-2012 Centreon
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

#include "com/centreon/broker/neb/check.hh"

using namespace com::centreon::broker::neb;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
check::check()
    : active_checks_enabled(false), check_type(0), host_id(0), next_check(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
check::check(check const& c) : io::data(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
check::~check() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
check& check::operator=(check const& c) {
  if (&c != this) {
    io::data::operator=(c);
    _internal_copy(c);
  }
  return (*this);
}

/**************************************
 *                                     *
 *          Private Methods            *
 *                                     *
 **************************************/

/**
 *  Copy internal data of the given object to the current instance.
 *
 *  @param[in] c Object to copy from.
 */
void check::_internal_copy(check const& c) {
  active_checks_enabled = c.active_checks_enabled;
  check_type = c.check_type;
  command_line = c.command_line;
  host_id = c.host_id;
  next_check = c.next_check;
  return;
}
