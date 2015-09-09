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

#include "com/centreon/broker/dumper/db_dump_committed.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
db_dump_committed::db_dump_committed() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
db_dump_committed::db_dump_committed(db_dump_committed const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
db_dump_committed::~db_dump_committed() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
db_dump_committed& db_dump_committed::operator=(db_dump_committed const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int db_dump_committed::type() const {
  return (io::events::data_type<io::events::dumper, dumper::de_db_dump_committed>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void db_dump_committed::_internal_copy(db_dump_committed const& other) {
  return ;
}
