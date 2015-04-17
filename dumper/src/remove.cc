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
  mapping::entry()
};

// Operations.
static io::data* new_remove() {
  return (new dumper::remove);
}
io::event_info::event_operations const dumper::remove::operations = {
  &new_remove
};
