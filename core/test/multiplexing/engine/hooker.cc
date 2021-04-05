/*
 * Copyright 2011 - 2021 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include "hooker.hh"

#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
hooker::hooker() {}

/**
 *  Destructor.
 */
hooker::~hooker() {}

/**
 *  Read events from the hook.
 *
 *  @param[out] d         Event.
 *  @param[in]  deadline  Unused.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool hooker::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  if (!_queue.empty()) {
    d = _queue.front();
    _queue.pop();
  } else if (!_registered)
    throw msg_fmt("hooker test object is shutdown");
  return true;
}

/**
 *  Multiplexing engine is starting.
 */
void hooker::starting() {
  std::shared_ptr<io::raw> raw(new io::raw);
  raw->append(HOOKMSG1);
  _queue.push(std::static_pointer_cast<io::data>(raw));
}

/**
 *  Multiplexing engine is stopping.
 */
void hooker::stopping() {
  std::shared_ptr<io::raw> raw(new io::raw);
  raw->append(HOOKMSG3);
  _queue.push(std::static_pointer_cast<io::data>(raw));
}

/**
 *  Receive event to hook.
 *
 *  @param[in] d Ignored.
 *
 *  @return Number of elements acknowledged (1).
 */
int32_t hooker::write(std::shared_ptr<io::data> const& d) {
  (void)d;
  if (_registered) {
    std::shared_ptr<io::raw> raw(new io::raw);
    raw->append(HOOKMSG2);
    _queue.push(std::static_pointer_cast<io::data>(raw));
  } else
    throw msg_fmt("hooker test object is shutdown");
  return 1;
}

int32_t hooker::stop() {
  return 0;
}
