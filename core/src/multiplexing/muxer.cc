/*
** Copyright 2009-2013,2015 Centreon
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
  : _name(name),
    _persistent(persistent),
    _total_events(0) {
  // Load head queue file back in memory.
  if (_persistent) {
    try {
      std::auto_ptr<io::stream>
        mf(new persistent_file(_memory_file()));
      misc::shared_ptr<io::data> e;
      while (true) {
        e.clear();
        mf->read(e, 0);
        publish(e);
      }
    }
    catch (io::exceptions::shutdown const& e) {
      // Queue file was properly read back in memory.
      (void)e;
    }
  }

  // Load temporary file back in memory.
  {
    _temporary.reset(new persistent_file(_queue_file()));
    misc::shared_ptr<io::data> e;
    do {
      e.clear();
      if (!_get_event_from_temporary(e))
        // All temporary event was loaded into the memory event queue.
        // The recovery mode is disable.
        break ;

      // Push temporary event to the memory event queue.
      publish(e);
    } while (_total_events < event_queue_max_size());
  }

  // Log messages.
  logging::info(logging::low)
    << "multiplexing: '" << _name << "' start with " << _total_events
    << " in queue and the queue file is "
    << (_temporary.get() ? "enable" : "disable");
}

/**
 *  Destructor.
 */
muxer::~muxer() {
  _clean();
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
    if (_total_events >= event_queue_max_size()) {
      // Try to create temporary if is necessary.
      if (!_temporary.get())
        _temporary.reset(new persistent_file(_queue_file()));
      _temporary->write(event);
    }
    else {
      _events.push(event);
      _cv.wakeOne();
    }
    ++_total_events;
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
  if (_events.empty()) {
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
    if (!_events.empty()) {
      _get_last_event(event);
      lock.unlock();
      if (!event.isNull())
        timed_out = false;
    }
    else
      event.clear();
  }
  // Data is available, no need to wait.
  else {
    _get_last_event(event);
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
 *  Generate statistics about the subscriber.
 *
 *  @param[out] buffer Output buffer.
 */
void muxer::statistics(io::properties& tree) const {
  // Lock object.
  QMutexLocker lock(&_mutex);

  // Temporary mode.
  tree.add_property(
         "queue_file_enabled",
         io::property("queue file enabled", _temporary.get() ? "yes" : "no"));

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
  if (_temporary.get())
    _temporary.reset();
  if (_persistent && !_events.empty()) {
    try {
      std::auto_ptr<io::stream>
        mf(new persistent_file(_memory_file()));
      while (!_events.empty()) {
        mf->write(_events.front());
        _events.pop();
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "multiplexing: could not backup memory queue of '" << _name
        << "': " << e.what();
    }
  }
  while (!_events.empty())
    _events.pop();
  _total_events = 0;
  return ;
}

/**
 *  Get event from temporary file. Warning, lock
 *  _mutex before use this function.
 *
 *  @param[out] event The Last event available.
 *
 *  @return True if have event into the temporary.
 */
bool muxer::_get_event_from_temporary(
              misc::shared_ptr<io::data>& event) {
  bool ret(false);
  event.clear();
  // If temporary exist, try to get the last event.
  if (_temporary.get()) {
    try {
      do {
        _temporary->read(event);
      } while (event.isNull());
      ret = true;
    }
    catch (io::exceptions::shutdown const& e) {
      // The temporary end was reach.
      (void)e;
      _temporary.reset();
      _total_events = _events.size();
    }
  }
  return (ret);
}

/**
 *  Get the last event available from the internal queue. Warning, lock
 *  _mutex before use this function.
 *
 *  @param[out] event Last event available.
 */
void muxer::_get_last_event(misc::shared_ptr<io::data>& event) {
  // Get the last avaiable event.
  event = _events.front();
  _events.pop();
  --_total_events;

  // Try to get the last temporary event.
  misc::shared_ptr<io::data> e;
  if (_get_event_from_temporary(e))
    _events.push(e);

  return ;
}

/**
 *  Get memory file path.
 *
 *  @return Path to the memory file.
 */
std::string muxer::_memory_file() const {
  std::string retval(config::applier::state::instance().cache_dir());
  retval.append(".memory.");
  retval.append(_name);
  return (retval);
}

/**
 *  Get queue file path.
 *
 *  @return Path to the queue file.
 */
std::string muxer::_queue_file() const {
  std::string retval(config::applier::state::instance().cache_dir());
  retval.append(".queue.");
  retval.append(_name);
  return (retval);
}
