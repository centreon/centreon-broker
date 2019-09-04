/*
** Copyright 2012-2013,2015 Centreon
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

#include "temporary_stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] id The temporary id.
 */
temporary_stream::temporary_stream(std::string const& id) : _id(id) {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
temporary_stream::temporary_stream(temporary_stream const& ss)
    : io::stream(ss) {
  _events = ss._events;
  _id = ss._id;
}

/**
 *  Destructor.
 */
temporary_stream::~temporary_stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
temporary_stream& temporary_stream::operator=(temporary_stream const& ss) {
  if (this != &ss) {
    io::stream::operator=(ss);
    std::lock_guard<std::mutex> lock1(_eventsm);
    std::lock_guard<std::mutex> lock2(ss._eventsm);
    _events = ss._events;
    _id = ss._id;
  }
  return *this;
}

/**
 *  Read some data.
 *
 *  @param[out] data      Some data.
 *  @param[in]  deadline  Unused.
 *
 *  @return Always return true.
 */
bool temporary_stream::read(std::shared_ptr<io::data>& data, time_t deadline) {
  (void)deadline;
  std::lock_guard<std::mutex> lock(_eventsm);
  if (_events.empty())
    throw exceptions::msg() << "temporary stream does not have any more event";
  else
    data = _events.front();
  _events.pop();
  return true;
}

/**
 *  Write some data.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of elements acknowledged (1).
 */
int temporary_stream::write(std::shared_ptr<io::data> const& d) {
  std::lock_guard<std::mutex> lock(_eventsm);
  _events.push(d);
  return 1;
}
