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

#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/events.hh"

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
raw::raw(raw const& r) : io::data(r), _buffer{r._buffer} {}

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
  if (this != &r) {
    data::operator=(r);
    _buffer = r._buffer;
  }
  return *this;
}

/**
 *  Get the data type.
 *
 *  @return Raw data.
 */
uint32_t raw::type() const {
  return raw::static_type();
}

/**
 *  Get the data type.
 *
 *  @return Raw data.
 */
uint32_t raw::static_type() {
  return events::data_type<events::internal, events::de_raw>::value;
}

void raw::resize(size_t s) {
  _buffer.resize(s);
}

char* raw::data() {
  return &_buffer[0];
}

char const* raw::const_data() const {
  return &_buffer[0];
}

size_t raw::size() const {
  return _buffer.size();
}

std::vector<char>& raw::get_buffer() {
  return _buffer;
}

bool raw::empty() const {
  return _buffer.empty();
}

void raw::append(std::string const& msg) {
  _buffer.insert(_buffer.end(), msg.begin(), msg.end());
}
