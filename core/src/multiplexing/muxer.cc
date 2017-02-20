/*
** Copyright 2009-2013,2015-2017 Centreon
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

#include <limits>
#include <memory>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/persistent_file.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

unsigned int muxer::_event_queue_max_size = std::numeric_limits<unsigned int>::max();

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] name        Name associated to this muxer. It is used to
 *                         create on-disk files.
 *  @param[in] persistent  Whether or not this muxer should backup
 *                         unprocessed events in a persistent storage.
 */
muxer::muxer(
         std::string const& name,
         bool persistent)
  : _events_size(0),
    _name(name),
    _persistent(persistent) {
  // Load head queue file back in memory.
  if (_persistent) {
    try {
      std::auto_ptr<io::stream>
        mf(new persistent_file(_memory_file()));
      misc::shared_ptr<io::data> e;
      while (true) {
        e.clear();
        mf->read(e, 0);
        if (!e.isNull()) {
          _events.push_back(e);
          ++_events_size;
        }
      }
    }
    catch (io::exceptions::shutdown const& e) {
      // Memory file was properly read back in memory.
      (void)e;
    }
  }

  // Load queue file back in memory.
  try {
    _file.reset(new persistent_file(_queue_file()));
    misc::shared_ptr<io::data> e;
    // The following do-while might read an extra event from the queue
    // file back in memory. However this is necessary to ensure that a
    // read() operation was done on the queue file and prevent it from
    // being open in case it is empty.
    do {
      e.clear();
      _get_event_from_file(e);
      if (e.isNull())
        break ;
      _push_to_queue(e);
    } while (_events_size < event_queue_max_size());
  }
  catch (io::exceptions::shutdown const& e) {
    // Queue file was entirely read back.
    (void)e;
  }
  _pos = _events.begin();

  // Log messages.
  logging::info(logging::low)
    << "multiplexing: '" << _name << "' start with " << _events_size
    << " in queue and the queue file is "
    << (_file.get() ? "enable" : "disable");
}

/**
 *  Destructor.
 */
muxer::~muxer() {
  _clean();
}

/**
 *  Acknowledge events.
 *
 *  @param[in] count  Number of events to acknowledge.
 */
void muxer::ack_events(int count) {
  // Remove acknowledged events.
  QMutexLocker lock(&_mutex);
  for (int i(0); (i < count) && !_events.empty(); ++i) {
    _events.pop_front();
    --_events_size;
  }

  // Fill memory from file.
  misc::shared_ptr<io::data> e;
  while (_events_size < event_queue_max_size()) {
    e.clear();
    _get_event_from_file(e);
    if (e.isNull())
      break ;
    _push_to_queue(e);
  }

  return ;
}

/**
 *  Set the maximume event queue size.
 *
 *  @param[in] max  The size limit.
 */
void muxer::event_queue_max_size(unsigned int max) throw () {
  if (!max)
    max = std::numeric_limits<unsigned int>::max();
  _event_queue_max_size = max;
  return ;
}

/**
 *  Get the maximum event queue size.
 *
 *  @return The size limit.
 */
unsigned int muxer::event_queue_max_size() throw () {
  return (_event_queue_max_size);
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 */
void muxer::publish(misc::shared_ptr<io::data> const& event) {
  if (!event.isNull()) {
    QMutexLocker lock(&_mutex);
    // Check if we should process this event.
    if (_write_filters.find(event->type()) == _write_filters.end())
      return ;
    // Check if the event queue limit is reach.
    if (_events_size >= event_queue_max_size()) {
      // Try to create file if is necessary.
      if (!_file.get())
        _file.reset(new persistent_file(_queue_file()));
      _file->write(event);
    }
    else
      _push_to_queue(event);
  }
  return ;
}

/**
 *  Get the next available event without waiting more than timeout.
 *
 *  @param[out] event      Next available event.
 *  @param[in]  deadline   Date limit.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool muxer::read(
              misc::shared_ptr<io::data>& event,
              time_t deadline) {
  bool timed_out(false);
  QMutexLocker lock(&_mutex);

  // No data is directly available.
  if (_pos == _events.end()) {
    // Wait a while if subscriber was not shutdown.
    if ((time_t)-1 == deadline)
      _cv.wait(&_mutex);
    else {
      time_t now(time(NULL));
      if (now < deadline)
        timed_out = !_cv.wait(&_mutex, (deadline - now) * 1000);
      else
        timed_out = true;
    }
    if (_pos != _events.end()) {
      event = *_pos;
      ++_pos;
      lock.unlock();
      if (!event.isNull())
        timed_out = false;
    }
    else
      event.clear();
  }
  // Data is available, no need to wait.
  else {
    event = *_pos;
    ++_pos;
    lock.unlock();
  }
  return (!timed_out);
}

/**
 *  Set the read filters.
 *
 *  @param[in] fltrs  Read filters. That is read() will return only
 *                    events which type() is available in this set.
 */
void muxer::set_read_filters(muxer::filters const& fltrs) {
  _read_filters = fltrs;
  return ;
}

/**
 *  Set the write filters.
 *
 *  @param[in] fltrs  Write filters. That is any submitted through
 *                    write() must be in this set otherwise it won't be
 *                    multiplexed.
 */
void muxer::set_write_filters(muxer::filters const& fltrs) {
  _write_filters = fltrs;
  return ;
}

/**
 *  Get the read filters.
 *
 *  @return  The read filters.
 */
muxer::filters const& muxer::get_read_filters() const {
  return (_read_filters);
}

/**
 *  Get the write filters.
 *
 *  @return  The write filters.
 */
muxer::filters const& muxer::get_write_filters() const {
  return (_write_filters);
}

/**
 *  Get the size of the event queue.
 *
 *  @return  The size of the event queue.
 */
unsigned int muxer::get_event_queue_size() const {
  QMutexLocker lock(&_mutex);
  return (_events.size());
}

/**
 *  Reprocess non-acknowledged events.
 */
void muxer::nack_events() {
  QMutexLocker lock(&_mutex);
  _pos = _events.begin();
  return ;
}

/**
 *  Generate statistics about the subscriber.
 *
 *  @param[out] buffer Output buffer.
 */
void muxer::statistics(io::properties& tree) const {
  // Lock object.
  QMutexLocker lock(&_mutex);

  // Queue file mode.
  tree.add_property(
         "queue_file_enabled",
         io::property("queue file enabled", _file.get() ? "yes" : "no"));

  return ;
}

/**
 *  Wake all threads waiting on this subscriber.
 */
void muxer::wake() {
  QMutexLocker lock(&_mutex);
  _cv.wakeAll();
  return ;
}

/**
 *  Send an event to multiplexing.
 *
 *  @param[in] d  Event to multiplex.
 */
int muxer::write(misc::shared_ptr<io::data> const& d) {
  if (!d.isNull()
      && (_read_filters.find(d->type()) != _read_filters.end()))
    engine::instance().publish(d);
  return (1);
}

/**
 *  Get the memory file name associated with this muxer.
 *
 *  @param[in] name  Name of this muxer.
 *
 *  @return  The memory file name associated with this muxer.
 */
std::string muxer::memory_file(std::string const& name) {
  std::string retval(config::applier::state::instance().cache_dir());
  retval.append(".memory.");
  retval.append(name);
  return (retval);
}

/**
 *  Get the queue file name associated with this muxer.
 *
 *  @param[in] name  Name of this muxer.
 *
 *  @return  The queue file name associated with this muxer.
 */
std::string muxer::queue_file(std::string const& name) {
  std::string retval(config::applier::state::instance().cache_dir());
  retval.append(".queue.");
  retval.append(name);
  return (retval);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Release all events stored within the internal list.
 */
void muxer::_clean() {
  QMutexLocker lock(&_mutex);
  _file.reset();
  if (_persistent && !_events.empty()) {
    try {
      std::auto_ptr<io::stream>
        mf(new persistent_file(_memory_file()));
      while (!_events.empty()) {
        mf->write(_events.front());
        _events.pop_front();
        --_events_size;
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "multiplexing: could not backup memory queue of '" << _name
        << "': " << e.what();
    }
  }
  _events.clear();
  _events_size = 0;
  return ;
}

/**
 *  Get event from retention file. Warning: lock _mutex before using
 *  this function.
 *
 *  @param[out] event  Last event available. Null if none is available.
 */
void muxer::_get_event_from_file(misc::shared_ptr<io::data>& event) {
  event.clear();
  // If file exist, try to get the last event.
  if (_file.get()) {
    try {
      do {
        _file->read(event);
      } while (event.isNull());
    }
    catch (io::exceptions::shutdown const& e) {
      // The file end was reach.
      (void)e;
      _file.reset();
    }
  }
  return ;
}

/**
 *  Get memory file path.
 *
 *  @return Path to the memory file.
 */
std::string muxer::_memory_file() const {
  return (memory_file(_name));
}

/**
 *  Push event to queue.
 *
 *  @param[in] event  New event.
 */
void muxer::_push_to_queue(misc::shared_ptr<io::data> const& event) {
  bool pos_has_no_more_to_read(_pos == _events.end());
  _events.push_back(event);
  ++_events_size;
  if (pos_has_no_more_to_read) {
    _pos = --_events.end();
    _cv.wakeOne();
  }
  return ;
}

/**
 *  Get queue file path.
 *
 *  @return Path to the queue file.
 */
std::string muxer::_queue_file() const {
  return (queue_file(_name));
}
