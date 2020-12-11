/*
** Copyright 2014-2020 Centreon
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

#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_truncate_table_signal::dimension_truncate_table_signal(bool update)
    : io::data(dimension_truncate_table_signal::static_type()),
      update_started(update) {}

// Mapping.
mapping::entry const dimension_truncate_table_signal::entries[] = {
    mapping::entry(&bam::dimension_truncate_table_signal::update_started,
                   "update_started"),
    mapping::entry()};

// Operations.
static io::data* new_dimension_truncate_table_signal() {
  return new dimension_truncate_table_signal(true);
}
io::event_info::event_operations const
    dimension_truncate_table_signal::operations = {
        &new_dimension_truncate_table_signal};
