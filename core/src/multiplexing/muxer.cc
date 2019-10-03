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

#include "com/centreon/broker/multiplexing/muxer.hh"
#include <limits>
#include <memory>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/persistent_file.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

uint32_t muxer::_event_queue_max_size =
    std::numeric_limits<uint32_t>::max();

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
muxer::muxer(std::string const& name, bool persistent)
    : _events_size(0), _name(name), _persistent(persistent) {
  // Load head queue file back in memory.
  if (_persistent) {
    try {
      std::unique_ptr<io::stream> mf(new persistent_file(_memory_file()));
      std::shared_ptr<io::data> e;
      while (true) {
        e.reset();
        mf->read(e, 0);
        if (e) {
          _events.push_back(e);
          ++_events_size;
        }
      }
    } catch (exceptions::shutdown const& e) {
      // Memory file was properly read back in memory.
      (void)e;
    }
  }

  // Load queue file back in memory.
  try {
    _file.reset(new persistent_file(_queue_file()));
    std::shared_ptr<io::data> e;
    // The following do-while might read an extra event from the queue
    // file back in memory. However this is necessary to ensure that a
    // read() operation was done on the queue file and prevent it from
    // being open in case it is empty.
    do {
      e.reset();
      _get_event_from_file(e);
      if (!e)
        break;
      _events.push_back(e);
      ++_events_size;
    } while (_events_size < event_queue_max_size());
  } catch (exceptions::shutdown const& e) {
    // Queue file was entirely read back.
    (void)e;
  }
  _pos = _events.begin();

  // Log messages.
  logging::info(logging::low)
      << "multiplexing: '" << _name << "' start with " << _events_size
      << " in queue and the queue file is " << (_file ? "enable" : "disable");
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
  logging::debug(logging::low) << "multiplexing: acknowledging " << count
                               << " events from " << _name << " event queue";
  if (count) {
    std::lock_guard<std::mutex> lock(_mutex);
    for (int i(0); (i < count) && !_events.empty(); ++i) {
      if (_events.begin() == _pos) {
        logging::error(logging::high)
            << "multiplexing: attempt to "
            << "acknowledge more events than available in " << _name
            << " event queue: " << count << " requested, " << i
            << " acknowledged";
        break;
      }
      _events.pop_front();
      --_events_size;
    }

    // Fill memory from file.
    std::shared_ptr<io::data> e;
    while (_events_size < event_queue_max_size()) {
      e.reset();
      _get_event_from_file(e);
      if (!e)
        break;
      _push_to_queue(e);
    }
  }
}

/**
 *  Set the maximume event queue size.
 *
 *  @param[in] max  The size limit.
 */
void muxer::event_queue_max_size(uint32_t max) noexcept {
  if (!max)
    max = std::numeric_limits<uint32_t>::max();
  _event_queue_max_size = max;
}

/**
 *  Get the maximum event queue size.
 *
 *  @return The size limit.
 */
uint32_t muxer::event_queue_max_size() noexcept {
  return _event_queue_max_size;
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 */
void muxer::publish(std::shared_ptr<io::data> const& event) {
  if (event) {
    std::lock_guard<std::mutex> lock(_mutex);
    // Check if we should process this event.
    if (_write_filters.find(event->type()) == _write_filters.end())
      return;
    // Check if the event queue limit is reach.
    if (_events_size >= event_queue_max_size()) {
      // Try to create file if is necessary.
      if (!_file)
        _file.reset(new persistent_file(_queue_file()));
      _file->write(event);
    } else
      _push_to_queue(event);
  }
}

/**
 *  Get the next available event without waiting more than timeout.
 *
 *  @param[out] event      Next available event.
 *  @param[in]  deadline   Date limit.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool muxer::read(std::shared_ptr<io::data>& event, time_t deadline) {
  bool timed_out(false);
  std::unique_lock<std::mutex> lock(_mutex);

  // No data is directly available.
  if (_pos == _events.end()) {
    // Wait a while if subscriber was not shutdown.
    if ((time_t)-1 == deadline)
      _cv.wait(lock);
    else {
      time_t now(time(nullptr));
      timed_out = _cv.wait_for(lock, std::chrono::seconds(deadline - now)) ==
                  std::cv_status::timeout;
    }
    if (_pos != _events.end()) {
      event = *_pos;
      ++_pos;
      lock.unlock();
      if (event)
        timed_out = false;
    } else
      event.reset();
  }
  // Data is available, no need to wait.
  else {
    event = *_pos;
    ++_pos;
    lock.unlock();
  }

  return !timed_out;
}

/**
 *  Set the read filters.
 *
 *  @param[in] fltrs  Read filters. That is read() will return only
 *                    events which type() is available in this set.
 */
void muxer::set_read_filters(muxer::filters const& fltrs) {
  _read_filters = fltrs;
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
}

/**
 *  Get the read filters.
 *
 *  @return  The read filters.
 */
muxer::filters const& muxer::get_read_filters() const {
  return _read_filters;
}

/**
 *  Get the write filters.
 *
 *  @return  The write filters.
 */
muxer::filters const& muxer::get_write_filters() const {
  return _write_filters;
}

/**
 *  Get the size of the event queue.
 *
 *  @return  The size of the event queue.
 */
uint32_t muxer::get_event_queue_size() const {
  std::lock_guard<std::mutex> lock(_mutex);
  return _events.size();
}

/**
 *  Reprocess non-acknowledged events.
 */
void muxer::nack_events() {
  logging::debug(logging::low)
      << "multiplexing: reprocessing unacknowledged events from " << _name
      << " event queue";
  std::lock_guard<std::mutex> lock(_mutex);
  _pos = _events.begin();
}

/**
 *  Generate statistics about the subscriber.
 *
 *  @param[out] buffer Output buffer.
 */
void muxer::statistics(json11::Json::object& tree) const {
  // Lock object.
  std::lock_guard<std::mutex> lock(_mutex);

  // Queue file mode.
  bool queue_file_enabled(_file.get());
  tree["queue_file_enabled"] = queue_file_enabled == true;
  if (queue_file_enabled) {
    json11::Json::object queue_file;
    _file->statistics(queue_file);
    tree["queue_file"] = queue_file;
  }

  // Unacknowledged events count.
  int unacknowledged(0);
  for (std::list<std::shared_ptr<io::data> >::const_iterator it(
           _events.begin());
       it != _pos; ++it)
    ++unacknowledged;
  tree["unacknowledged_events"] = unacknowledged;
}

/**
 *  Wake all threads waiting on this subscriber.
 */
void muxer::wake() {
  std::lock_guard<std::mutex> lock(_mutex);
  _cv.notify_all();
}

/**
 *  Send an event to multiplexing.
 *
 *  @param[in] d  Event to multiplex.
 */
int muxer::write(std::shared_ptr<io::data> const& d) {
  if (d && _read_filters.find(d->type()) != _read_filters.end())
    engine::instance().publish(d);
  return 1;
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
  return retval;
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
  return retval;
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
  std::lock_guard<std::mutex> lock(_mutex);
  _file.reset();
  if (_persistent && !_events.empty()) {
    try {
      std::unique_ptr<io::stream> mf(new persistent_file(_memory_file()));
      while (!_events.empty()) {
        mf->write(_events.front());
        _events.pop_front();
        --_events_size;
      }
    } catch (std::exception const& e) {
      logging::error(logging::high)
          << "multiplexing: could not backup memory queue of '" << _name
          << "': " << e.what();
    }
  }
  _events.clear();
  _events_size = 0;
}

/**
 *  Get event from retention file. Warning: lock _mutex before using
 *  this function.
 *
 *  @param[out] event  Last event available. Null if none is available.
 */
void muxer::_get_event_from_file(std::shared_ptr<io::data>& event) {
  event.reset();
  // If file exist, try to get the last event.
  if (_file) {
    try {
      do {
        _file->read(event);
      } while (!event);
    } catch (exceptions::shutdown const& e) {
      // The file end was reach.
      (void)e;
      _file.reset();
    }
  }
}

/**
 *  Get memory file path.
 *
 *  @return Path to the memory file.
 */
std::string muxer::_memory_file() const {
  return memory_file(_name);
}

/**
 *  Push event to queue.
 *
 *  @param[in] event  New event.
 */
void muxer::_push_to_queue(std::shared_ptr<io::data> const& event) {
  bool pos_has_no_more_to_read(_pos == _events.end());
  _events.push_back(event);
  ++_events_size;

  if (pos_has_no_more_to_read) {
    _pos = --_events.end();
    _cv.notify_one();
  }
}

/**
 *  Get queue file path.
 *
 *  @return Path to the queue file.
 */
std::string muxer::_queue_file() const {
  return queue_file(_name);
}

/**
 *  Remove all the queue files attached to this muxer.
 */
void muxer::remove_queue_files() {
  logging::info(logging::low)
      << "multiplexing: '" << _queue_file() << "' removed";

  /* Here _file is already destroyed */
  persistent_file file(_queue_file());
  file.remove_all_files();
}
