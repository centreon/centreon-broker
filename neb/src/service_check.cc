/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/neb/service_check.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
service_check::service_check()
    : check(service_check::static_type()), service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] service_check Object to copy.
 */
service_check::service_check(service_check const& sc)
    : check(sc), service_id(sc.service_id) {}

/**
 *  Destructor.
 */
service_check::~service_check() {}

/**
 *  Assignment operator.
 *
 *  @param[in] sc Object to copy.
 *
 *  @return This object.
 */
service_check& service_check::operator=(service_check const& sc) {
  check::operator=(sc);
  service_id = sc.service_id;
  return *this;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const service_check::entries[] = {
    mapping::entry(&service_check::active_checks_enabled, ""),
    mapping::entry(&service_check::check_type, ""),
    mapping::entry(&service_check::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&service_check::next_check, ""),
    mapping::entry(&service_check::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&service_check::command_line, "command_line"),
    mapping::entry()};

// Operations.
static io::data* new_service_check() {
  return new service_check;
}
io::event_info::event_operations const service_check::operations = {
    &new_service_check};
