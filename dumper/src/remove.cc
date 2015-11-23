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
#include "com/centreon/broker/dumper/remove.hh"
#include "com/centreon/broker/dumper/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Default constructor.
 */
dumper::remove::remove() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
dumper::remove::remove(remove const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
dumper::remove::~remove() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
dumper::remove& dumper::remove::operator=(remove const& right) {
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
unsigned int dumper::remove::type() const {
  return (remove::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int dumper::remove::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_remove>::value);
}


/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void dumper::remove::_internal_copy(remove const& right) {
  tag = right.tag;
  filename = right.filename;
  req_id = right.req_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dumper::remove::entries[] = {
  mapping::entry(
    &dumper::remove::tag,
    "tag"),
  mapping::entry(
    &dumper::remove::filename,
    "filename"),
  mapping::entry(
    &dumper::remove::req_id,
    "req_id"),
  mapping::entry()
};

// Operations.
static io::data* new_remove() {
  return (new dumper::remove);
}
io::event_info::event_operations const dumper::remove::operations = {
  &new_remove
};
