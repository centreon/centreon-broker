/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/bam/bool_value.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_value::bool_value() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_value::bool_value(bool_value const& right) : computable(right) {}

/**
 *  Destructor.
 */
bool_value::~bool_value() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_value& bool_value::operator=(bool_value const& right) {
  computable::operator=(right);
  return (*this);
}

/**
 *  Is this boolean value in downtime?
 *
 *  @return  True if this boolean value is in downtime.
 */
bool bool_value::in_downtime() const {
  return (false);
}
