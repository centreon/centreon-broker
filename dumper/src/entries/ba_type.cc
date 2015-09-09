/*
** Copyright 2015 Centreon
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
ba_type::ba_type() : enable(true), ba_type_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba_type::ba_type(ba_type const& other)
  : io::data(other),
    enable(other.enable),
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
    enable = other.enable;
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
          && (enable == other.enable)
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
    &ba_type::enable,
    ""),
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
