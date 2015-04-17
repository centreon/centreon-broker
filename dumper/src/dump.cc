/*
** Copyright 2013,2015 Merethis
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Default constructor.
 */
dump::dump() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
dump::dump(dump const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
dump::~dump() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
dump& dump::operator=(dump const& right) {
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
unsigned int dump::type() const {
  return (dump::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int dump::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_dump>::value);
}


/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void dump::_internal_copy(dump const& right) {
  content = right.content;
  tag = right.tag;
  filename = right.filename;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dump::entries[] = {
  mapping::entry(
    &dump::content,
    "content"),
  mapping::entry(
    &dump::tag,
    "tag"),
  mapping::entry(
    &dump::filename,
    "filename"),
  mapping::entry()
};

// Operations.
static io::data* new_dump() {
  return (new dump);
}
io::event_info::event_operations const dump::operations = {
  &new_dump
};
