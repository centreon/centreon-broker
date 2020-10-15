/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/rebuild.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 * @brief The default constructor
 */
rebuild::rebuild() : io::data(rebuild::static_type()) {}

/**
 * @brief The best constructor with the ba ids to rebuild in a string
 *
 * @param lst A string containing the list of bas to rebuild (separated by
 * commas)
 */
rebuild::rebuild(const std::string& lst)
    : io::data(rebuild::static_type()), bas_to_rebuild(lst) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
rebuild::rebuild(rebuild const& other)
    : io::data(other), bas_to_rebuild(other.bas_to_rebuild) {}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test for equality.
 *
 *  @return  True if the two objects are equal.
 */
bool rebuild::operator==(rebuild const& other) const {
  return bas_to_rebuild == other.bas_to_rebuild;
}

// Mapping.
mapping::entry const rebuild::entries[] = {
    mapping::entry(&bam::rebuild::bas_to_rebuild, "bas_to_rebuild", 0),
    mapping::entry()};

// Operations.
static io::data* new_rebuild() {
  return new rebuild;
}
io::event_info::event_operations const rebuild::operations = {&new_rebuild};
