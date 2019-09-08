/*
** Copyright 2009-2011,2017 Centreon
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

#include "com/centreon/broker/exceptions/corruption.hh"

using namespace com::centreon::broker::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
corruption::corruption() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to build from.
 */
corruption::corruption(corruption const& other)
    : com::centreon::broker::exceptions::msg(other) {}

/**
 *  Destructor.
 */
corruption::~corruption() throw() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] other  Object to copy from.
 *
 *  @return This instance.
 */
corruption& corruption::operator=(corruption const& other) {
  com::centreon::broker::exceptions::msg::operator=(other);
  return (*this);
}
