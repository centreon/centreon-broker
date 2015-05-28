/*
** Copyright 2009-2013,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <ctime>
#include <limits>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/internal.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/persistent_file.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

unsigned int subscriber::_event_queue_max_size = std::numeric_limits<unsigned int>::max();

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] name        Name associated to this subscriber. It is
 *                         used to create on-disk files.
 *  @param[in] temp_dir    Directory in which temporary files will be
 *                         created.
 *  @param[in] persistent  Whether or not this subscriber should backup
 *                         unprocessed events in a persistent storage.
 */
subscriber::subscriber(
              std::string const& name,
              std::string const& temp_dir,
              bool persistent)
  : _name(name),
    _persistent(persistent),
    _process_in(true),
    _process_out(true),
    _recovery_temporary(false),
    _temp_dir(temp_dir),
    _total_events(0) {
  {
    QMutexLocker lock2(&_mutex);

    // Register self in subscriber list.
    {
      QMutexLocker lock1(&gl_subscribersm);
      gl_subscribers.push_back(this);
    }

    // Load head queue file back in memory.
    if (_persistent) {
      try {
        misc::shared_ptr<io::stream>
          mf(new persistent_file(_memory_file()));
        misc::shared_ptr<io::data> e;
        while (true) {
          mf->read(e);
          _events.enqueue(e);
          ++_total_events;
        }
      }
      catch (io::exceptions::shutdown const& e) {
        // Queue file was properly read back in memory.
        (void)e;
      }
    }

    // Load temporary file back in memory.
    {
      _temporary = new persistent_file(_queue_file());
      misc::shared_ptr<io::data> event;
      do {
        _recovery_temporary = _get_event_from_temporary(event);
        if (!_recovery_temporary) {
          // All temporary event was loaded into the memory event queue.
          // The recovery mode is disable.
          break ;
        }
        else {
          // Push temporary event to the memory event queue.
          _events.enqueue(event);
          ++_total_events;
        }
      } while (_total_events < event_queue_max_size());
    }
  }

  // Log messages.
  logging::info(logging::low)
    << "multiplexing: '" << _name << "' start with " << _total_events
    << " in queue and the queue file is "
    << (_recovery_temporary ? "enable" : "disable");
}

/**
 *  Destructor.
 */
subscriber::~subscriber() {
  clean();
  QMutexLocker lock(&gl_subscribersm);
  for (QVector<subscriber*>::iterator it(gl_subscribers.begin());
       it != gl_subscribers.end();)
    if (*it == this)
      it = gl_subscribers.erase(it);
    else
      ++it;
}

/**
 *  Set the maximume event queue size.
 *
 *  @param[in] max The size limit.
 */
void subscriber::event_queue_max_size(unsigned int max) throw () {
  if (!max)
    max = std::numeric_limits<unsigned int>::max();
  _event_queue_max_size = max;
}

/**
 *  Get the maximum event queue size.
 *
 *  @return The size limit.
 */
unsigned int subscriber::event_queue_max_size() throw () {
  return (_event_queue_max_size);
}

/**
 *  Get the next available event.
 *
 *  @param[out] d Next available event.
 */
void subscriber::read(misc::shared_ptr<io::data>& d) {
  this->read(d, (time_t)-1);
  return ;
}

/**
 *  Get the next available event without waiting more than timeout.
 *
 *  @param[out] event     Next available event.
 *  @param[in]  timeout   Date limit.
 *  @param[out] timed_out Set to true if read timed out.
 */
void subscriber::read(
                   misc::shared_ptr<io::data>& event,
                   time_t timeout,
                   bool* timed_out) {
  QMutexLocker lock(&_mutex);

  // No data is directly available.
  if (!_total_events) {
    // Wait a while if subscriber was not shutdown.
    if (_process_in && _process_out) {
      if ((time_t)-1 == timeout)
        _cv.wait(&_mutex);
      else {
        time_t now(time(NULL));
        if (now < timeout) {
          bool timedout(!_cv.wait(&_mutex, (timeout - now) * 1000));
          if (timed_out)
            *timed_out = timedout;
        }
        else if (timed_out)
          *timed_out = true;
      }
      if (_total_events) {
        _get_last_event(event);
        lock.unlock();
        logging::debug(logging::low) << "multiplexing: "
          << _total_events << " events remaining in subcriber";
      }
      else
        event.clear();
    }
    // If subscriber is shutdown, notify caller.
    else
      throw (io::exceptions::shutdown(true, true)
             << "thread is shutdown, cannot get any further event");
  }
  // Data is available, no need to wait.
  else {
    if (!_process_in && _process_out)
      throw (io::exceptions::shutdown(true, false)
             << "thread is shutdown, cannot get any further event");
    _get_last_event(event);
    lock.unlock();
    logging::debug(logging::low) << "multiplexing: " << _total_events
      << " events remaining in subscriber";
  }
  return ;
}

/**
 *  Set the filters.
 *
 *  @param[in] filters Content filters.
 */
void subscriber::set_filters(std::set<unsigned int> const& filters) {
  _filters = filters;
  return ;
}

/**
 *  Generate statistics about the subscriber.
 *
 *  @param[out] buffer Output buffer.
 */
void subscriber::statistics(io::properties& tree) const {
  // Lock object.
  QMutexLocker lock(&_mutex);

  // Queued events.
  std::ostringstream oss;
  {
    if (_recovery_temporary)
      oss << "queued events=unkown";
    else
      oss << "queued events=" << _total_events;
    io::property& p(tree["queued_events"]);
    p.set_perfdata(oss.str());
    p.set_graphable(false);
  }

  // Temporary mode.
  {
    oss.str("");
    char const* enable(_recovery_temporary ? "yes" : "no");
    oss << "temporary recovery mode=" << enable;
    io::property& p(tree["temporary_recovery_mode"]);
    p.set_perfdata(oss.str());
    p.set_graphable(false);
  }

  // Filters.
  {
    oss.str("");

    // Get numeric event categories.
    std::set<unsigned short> numcats;
    for (std::set<unsigned int>::const_iterator
           it(_filters.begin()),
           end(_filters.end());
         it != end;
         ++it)
      numcats.insert(io::events::category_of_type(*it));

    // Print filters.
    oss << "accepted events";
    if (numcats.empty()) {
      oss << "\n  all";
    }
    else {
      // Convert numeric categories to strings.
      for (std::set<unsigned short>::const_iterator
             it(numcats.begin()),
             end(numcats.end());
           it != end;
           ++it) {
        io::events::categories_container::const_iterator
          it2(io::events::instance().begin()),
          end2(io::events::instance().end());
        while (it2 != end2) {
          if (!it2->second.events.empty() && (*it == it2->first)) {
            oss << "\n  " << it2->second.name;
            break ;
          }
          ++it2;
        }
        if (it2 == end2)
          oss << "\n  " << *it;
      }
    }

    // Set property.
    io::property& p(tree["accepted_events"]);
    p.set_perfdata(oss.str());
    p.set_graphable(false);
  }

  return ;
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 *
 *  @return Number of elements acknowledged (1).
 */
unsigned int subscriber::write(misc::shared_ptr<io::data> const& event) {
  {
    QMutexLocker lock(&_mutex);
    // Check if we should process this event.
    if (!_filters.empty()
        && !event.isNull()
        && (_filters.find(event->type()) == _filters.end()))
      return (1);
    // Check if the event queue limit is reach.
    if (_total_events >= event_queue_max_size()) {
      // Try to create temporary if is necessary.
      if (!_temporary)
        _temporary = new persistent_file(_queue_file());

      // Check if we have temporary.
      if (_temporary)
        _temporary->write(event);
      else
        _events.enqueue(event);
    }
    else
      _events.enqueue(event);

    // If the recovery mode is disable increase total events.
    if (!_recovery_temporary)
      ++_total_events;
  }
  _cv.wakeOne();
  return (1);
}

/**
 *  Wake all threads waiting on this subscriber.
 */
void subscriber::wake() {
  QMutexLocker lock(&_mutex);
  _cv.wakeAll();
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Release all events stored within the internal list.
 */
void subscriber::clean() {
  QMutexLocker lock(&_mutex);
  if (_temporary)
    _temporary = misc::shared_ptr<io::stream>();
  if (_persistent) {
    try {
      misc::shared_ptr<io::stream>
        mf(new persistent_file(_memory_file()));
      while (!_events.empty())
        mf->write(_events.dequeue());
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "multiplexing: could not backup memory queue of '" << _name
        << "': " << e.what();
    }
  }
  _events.clear();
  _recovery_temporary = false;
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
bool subscriber::_get_event_from_temporary(
                   misc::shared_ptr<io::data>& event) {
  bool ret(false);
  // If temporary exist, try to get the last event.
  if (_temporary) {
    try {
      do {
        _temporary->read(event);
      } while (event.isNull());
      ret = true;
    }
    catch (io::exceptions::shutdown const& e) {
      // The temporary end was reach.
      (void)e;
      _temporary.clear();
      _recovery_temporary = false;
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
void subscriber::_get_last_event(misc::shared_ptr<io::data>& event) {
  // Try to get the last temporary event.
  if (_get_event_from_temporary(event))
    _events.enqueue(event);

  // If the recovery mode is disable decrease total events.
  if (!_recovery_temporary)
    --_total_events;

  // Get the last avaiable event.
  event = _events.dequeue();
  return ;
}

/**
 *  Get memory file path.
 *
 *  @return Path to the memory file.
 */
std::string subscriber::_memory_file() const {
  std::string retval(_temp_dir);
  retval.append("memory-");
  retval.append(_name);
  return (retval);
}

/**
 *  Get queue file path.
 *
 *  @return Path to the queue file.
 */
std::string subscriber::_queue_file() const {
  std::string retval(_temp_dir);
  retval.append("queue-");
  retval.append(_name);
  return (retval);
}
