/*
** Copyright 2015 Merethis
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

#include <cmath>
#include "com/centreon/broker/dumper/entries/organization.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
organization::organization()
  : enable(true),
    organization_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
organization::organization(organization const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
organization::~organization() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
organization& organization::operator=(organization const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are equal.
 */
bool organization::operator==(organization const& other) const {
  return ((enable == other.enable)
          && (name == other.name)
          && (organization_id == other.organization_id)
          && (shortname == other.shortname));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are not equal.
 */
bool organization::operator!=(organization const& other) const {
  return (!operator==(other));
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int organization::type() const {
  return (static_type());
}

/**
 *  Static type of event class.
 *
 *  @return Static type of event class.
 */
unsigned int organization::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_entries_organization>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data objects.
 *
 *  @param[in] other  Object to copy.
 */
void organization::_internal_copy(organization const& other) {
  enable = other.enable;
  name = other.name;
  organization_id = other.organization_id;
  shortname = other.shortname;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const organization::entries[] = {
  mapping::entry(
    &organization::enable,
    ""),
  mapping::entry(
    &organization::name,
    "name"),
  mapping::entry(
    &organization::organization_id,
    "organization_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &organization::shortname,
    "shortname"),
  mapping::entry()
};

// Operations.
static io::data* new_organization() {
  return (new organization);
}
io::event_info::event_operations const organization::operations = {
  &new_organization
};
