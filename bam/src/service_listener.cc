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

#include "com/centreon/broker/bam/service_listener.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
service_listener::service_listener() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
service_listener::service_listener(service_listener const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
service_listener::~service_listener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
service_listener& service_listener::operator=(
                                      service_listener const& right) {
  (void)right;
  return (*this);
}
