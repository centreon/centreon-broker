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

#include "com/centreon/broker/dumper/entries/ba_type.hh"
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
ba_type::ba_type() : ba_type_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba_type::ba_type(ba_type const& other)
  : io::data(other),
    ba_type_id(other.ba_type_id),
    description(other.description),
    name(other.name),
    slug(other.slug) {}

/**
 *  Destructor.
 */
ba_type::~ba_type() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ba_type& ba_type::operator=(ba_type const& other) {
  if (this != &other) {
    io::data::operator=(other);
    ba_type_id = other.ba_type_id;
    description = other.description;
    name = other.name;
    slug = other.slug;
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
bool ba_type::operator==(ba_type const& other) const {
  return ((ba_type_id == other.ba_type_id)
          && (description == other.description)
          && (name == other.name)
          && (slug == other.slug));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return False if both objects are equal.
 */
bool ba_type::operator!=(ba_type const& other) const {
  return (!operator==(other));
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int ba_type::type() const {
  return (static_type());
}

/**
 *  Return class type.
 *
 *  @return Class type.
 */
unsigned int ba_type::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_entries_ba_type>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const ba_type::entries[] = {
  mapping::entry(
    &ba_type::ba_type_id,
    "ba_type_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &ba_type::description,
    "description"),
  mapping::entry(
    &ba_type::name,
    "name"),
  mapping::entry(
    &ba_type::slug,
    "slug"),
  mapping::entry()
};

// Operations.
static io::data* new_ba_type() {
  return (new ba_type);
}
io::event_info::event_operations const ba_type::operations = {
  &new_ba_type
};
