/*
** Copyright 2011-2013,2015 Centreon
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

#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "test/multiplexing/engine/hooker.hh"

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
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
hooker::hooker(hooker const& other)
  : multiplexing::hooker(other), _queue(other._queue) {}

/**
 *  Destructor.
 */
hooker::~hooker() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
hooker& hooker::operator=(hooker const& other) {
  if (this != &other) {
    multiplexing::hooker::operator=(other);
    _queue = other._queue;
  }
  return (*this);
}

/**
 *  Read events from the hook.
 *
 *  @param[out] d         Event.
 *  @param[in]  deadline  Unused.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool hooker::read(
               misc::shared_ptr<io::data>& d,
               time_t deadline) {
  (void)deadline;
  d.clear();
  if (!_queue.isEmpty()) {
    d = _queue.head();
    _queue.dequeue();
  }
  else if (!_registered)
    throw (io::exceptions::shutdown(true, true)
             << "hooker test object is shutdown");
  return (true);
}

/**
 *  Multiplexing engine is starting.
 */
void hooker::starting() {
  misc::shared_ptr<io::raw> raw(new io::raw);
  raw->append(HOOKMSG1);
  _queue.enqueue(raw.staticCast<io::data>());
  return ;
}

/**
 *  Multiplexing engine is stopping.
 */
void hooker::stopping() {
  misc::shared_ptr<io::raw> raw(new io::raw);
  raw->append(HOOKMSG3);
  _queue.enqueue(raw.staticCast<io::data>());
  return ;
}

/**
 *  Receive event to hook.
 *
 *  @param[in] d Ignored.
 *
 *  @return Number of elements acknowledged (1).
 */
int hooker::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  if (_registered) {
    misc::shared_ptr<io::raw> raw(new io::raw);
    raw->append(HOOKMSG2);
    _queue.enqueue(raw.staticCast<io::data>());
  }
  else
    throw (io::exceptions::shutdown(true, true)
           << "hooker test object is shutdown");
  return (1);
}
