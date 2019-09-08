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

#include "com/centreon/broker/exceptions/config.hh"

using namespace com::centreon::broker::exceptions;

/**
 *  Default constructor.
 */
config::config() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
config::config(config const& other) : msg(other) {}

/**
 *  Destructor.
 */
config::~config() throw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
config& config::operator=(config const& other) {
  msg::operator=(other);
  return (*this);
}
