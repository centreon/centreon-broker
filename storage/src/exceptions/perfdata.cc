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

#include "com/centreon/broker/storage/exceptions/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
perfdata::perfdata() throw() {}

/**
 *  Copy constructor.
 *
 *  @param[in] pd Object to copy.
 */
perfdata::perfdata(perfdata const& pd) throw() : broker::exceptions::msg(pd) {}

/**
 *  Destructor.
 */
perfdata::~perfdata() throw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] pd Object to copy.
 *
 *  @return This object.
 */
perfdata& perfdata::operator=(perfdata const& pd) throw() {
  broker::exceptions::msg::operator=(pd);
  return (*this);
}

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
exceptions::msg* perfdata::clone() const {
  return (new perfdata(*this));
}

/**
 *  Rethrow the exception.
 */
void perfdata::rethrow() const {
  throw(*this);
  return;
}
