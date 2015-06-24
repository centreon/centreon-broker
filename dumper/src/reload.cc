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
  return (reload::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int dumper::reload::static_type() {
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

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dumper::reload::entries[] = {
  mapping::entry(
    &dumper::reload::tag,
    "tag"),
  mapping::entry()
};

// Operations.
static io::data* new_reload() {
  return (new dumper::reload);
}
io::event_info::event_operations const dumper::reload::operations = {
  &new_reload
};
