/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
status::status()
    : io::data(status::static_type()),
      ctime(0),
      index_id(0),
      interval(0),
      is_for_rebuild(false),
      rrd_len(0),
      state(0) {}

/**
 *  Constructor.
 *
 * @param ctime
 * @param index_id
 * @param interval
 * @param is_for_rebuild
 * @param rrd_len
 * @param state
 * @param Unknown
 */
status::status(timestamp const& ctime,
               uint32_t index_id,
               uint32_t interval,
               bool is_for_rebuild,
               timestamp const& rrd_len,
               int16_t state)
    : io::data(status::static_type()),
      ctime{ctime},
      index_id{index_id},
      interval{interval},
      is_for_rebuild{is_for_rebuild},
      rrd_len{rrd_len},
      state{state} {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
status::status(status const& s) : io::data(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
status::~status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
status& status::operator=(status const& s) {
  io::data::operator=(s);
  _internal_copy(s);
  return *this;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] s Object to copy.
 */
void status::_internal_copy(status const& s) {
  ctime = s.ctime;
  index_id = s.index_id;
  interval = s.interval;
  is_for_rebuild = s.is_for_rebuild;
  rrd_len = s.rrd_len;
  state = s.state;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const status::entries[] = {
    mapping::entry(&status::ctime,
                   "ctime",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&status::index_id,
                   "index_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&status::interval, "interval"),
    mapping::entry(&status::rrd_len, "rrd_len"),
    mapping::entry(&status::state, "state"),
    mapping::entry(&status::is_for_rebuild, "is_for_rebuild"),
    mapping::entry()};

// Operations.
static io::data* new_status() {
  return new status;
}
io::event_info::event_operations const status::operations = {&new_status};
