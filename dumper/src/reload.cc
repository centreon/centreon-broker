/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/dumper/reload.hh"
#include "com/centreon/broker/dumper/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Default constructor.
 */
dumper::reload::reload() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
dumper::reload::reload(reload const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
dumper::reload::~reload() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
dumper::reload& dumper::reload::operator=(reload const& right) {
  if (this != &right) {
    io::data::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int dumper::reload::type() const {
  return (io::events::data_type<io::events::dumper, dumper::de_reload>::value);
}


/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void dumper::reload::_internal_copy(reload const& right) {
  tag = right.tag;
  return ;
}
