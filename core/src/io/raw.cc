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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
raw::raw() {}

/**
 *  Copy constructor.
 *
 *  @param[in] r Object to copy.
 */
raw::raw(raw const& r) : io::data(r), QByteArray(r) {}

/**
 *  Destructor.
 */
raw::~raw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] r Object to copy.
 *
 *  @return This object.
 */
raw& raw::operator=(raw const& r) {
  data::operator=(r);
  QByteArray::operator=(r);
  return (*this);
}

/**
 *  Get the data type.
 *
 *  @return Raw data.
 */
unsigned int raw::type() const {
  return (events::data_type<events::internal, 1>::value);
}
