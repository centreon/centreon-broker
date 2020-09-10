/*
** Copyright 2011 Centreon
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

#include "com/centreon/broker/rrd/exceptions/open.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
open::open() noexcept {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
open::open(open const& o) noexcept : broker::exceptions::msg(o) {}

/**
 *  Destructor.
 */
open::~open() noexcept {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
exceptions::msg* open::clone() const {
  return (new open(*this));
}

/**
 *  Rethrow the exception.
 */
void open::rethrow() const {
  throw(*this);
  return;
}
