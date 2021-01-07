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

#include "com/centreon/broker/multiplexing/engine.hh"

#include <unistd.h>

#include <fmt/format.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <vector>

#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

// Class instance.
engine* engine::_instance(nullptr);
std::mutex engine::_load_m;

/**
 *  Clear events stored in the multiplexing engine.
 */
void engine::clear() {
  while (!_kiew.empty())
    _kiew.pop();
}

/**
 *  Set a hook.
 *
 *  @param[in] h          Hook.
 *  @param[in] with_data  Write data to hook.
 */
void engine::hook(hooker& h, bool with_data) {
  std::lock_guard<std::mutex> lock(_engine_m);

  _hooks.push_back({&h, with_data});
  _hooks_begin = _hooks.begin();
  _hooks_end = _hooks.end();
}

/**
 *  Get engine instance.
 *
 *  @return Class instance.
 */
engine& engine::instance() {
  assert(_instance);
  return *_instance;
}

/**
 *  Load engine instance.
 */
void engine::load() {
  std::lock_guard<std::mutex> lk(_load_m);
  if (!_instance)
    _instance = new engine;
}

/**
 *  Send an event to all subscribers.
 *
 *  @param[in] e  Event to publish.
 */
void engine::publish(std::shared_ptr<io::data> const& e) {
  // Lock mutex.
  std::lock_guard<std::mutex> lock(_engine_m);
  _publish(e);
}

void engine::publish(std::list<std::shared_ptr<io::data>> const& to_publish) {
  std::lock_guard<std::mutex> lock(_engine_m);
  for (auto& e : to_publish)
    _publish(e);
}

/**
 *  Send an event to all subscribers. It must be used from this class, and
 *  _engine_m must be locked previously. Otherwise use engine::publish().
 *
 *  @param[in] e  Event to publish.
 */
void engine::_publish(std::shared_ptr<io::data> const& e) {
  // Store object for further processing.
  _kiew.push(e);
  // Processing function.
  (this->*_write_func)(e);
}

/**
 *  Start multiplexing.
 */
void engine::start() {
  if (_write_func != &engine::_write) {
    // Set writing method.
    logging::debug(logging::high) << "multiplexing: starting";
    _write_func = &engine::_write;

    std::lock_guard<std::mutex> lock(_engine_m);
    // Local queue.
    std::queue<std::shared_ptr<io::data>> kiew;
    // Get events from the cache file to the local queue.
    try {
      persistent_cache cache(_cache_file_path());
      std::shared_ptr<io::data> d;
      while (true) {
        cache.get(d);
        if (!d)
          break;
        kiew.push(d);
      }
    } catch (std::exception const& e) {
      logging::error(logging::medium)
          << "multiplexing: couldn't read cache file: " << e.what();
    }

    // Copy global event queue to local queue.
    while (!_kiew.empty()) {
      kiew.push(_kiew.front());
      _kiew.pop();
    }

    // Notify hooks of multiplexing loop start.
    for (std::vector<std::pair<hooker*, bool>>::iterator it(_hooks_begin),
         end(_hooks_end);
         it != end; ++it) {
      it->first->starting();

      // Read events from hook.
      try {
        std::shared_ptr<io::data> d;
        it->first->read(d);
        while (d) {
          _kiew.push(d);
          it->first->read(d, 0);
        }
      } catch (std::exception const& e) {
        logging::error(logging::low)
            << "multiplexing: cannot read from hook: " << e.what();
      }
    }

    // Process events from hooks.
    _send_to_subscribers();

    // Send events queued while multiplexing was stopped.
    while (!kiew.empty()) {
      _publish(kiew.front());
      kiew.pop();
    }
  }
}

/**
 *  Stop multiplexing.
 */
void engine::stop() {
  if (_write_func != &engine::_nop) {
    // Notify hooks of multiplexing loop end.
    logging::debug(logging::high) << "multiplexing: stopping";
    std::unique_lock<std::mutex> lock(_engine_m);
    for (std::vector<std::pair<hooker*, bool>>::iterator it(_hooks_begin),
         end(_hooks_end);
         it != end; ++it) {
      it->first->stopping();

      // Read events from hook.
      try {
        std::shared_ptr<io::data> d;
        it->first->read(d);
        while (d) {
          _kiew.push(d);
          it->first->read(d);
        }
      } catch (...) {
      }
    }

    do {
      // Process events from hooks.
      _send_to_subscribers();

      // Make sur that no more data is available.
      lock.unlock();
      usleep(200000);
      lock.lock();
    } while (!_kiew.empty());

    // Open the cache file and start the transaction.
    // The cache file is used to cache all the events produced
    // while the engine is stopped. It will be replayed next time
    // the engine is started.
    try {
      _cache_file.reset(new persistent_cache(_cache_file_path()));
      _cache_file->transaction();
    } catch (std::exception const& e) {
      logging::error(logging::medium)
          << "multiplexing: could not open cache file: " << e.what();
      _cache_file.reset();
    }

    // Set writing method.
    _write_func = &engine::_write_to_cache_file;
  }
}

/**
 *  Subscribe to the multiplexing engine.
 *
 *  @param[in] subscriber  Subscriber.
 */
void engine::subscribe(muxer* subscriber) {
  std::lock_guard<std::mutex> lock(_muxers_m);
  _muxers.push_back(subscriber);
}

/**
 *  Remove a hook.
 *
 *  @param[in] h  Hook.
 */
void engine::unhook(hooker& h) {
  std::lock_guard<std::mutex> lock(_engine_m);
  for (std::vector<std::pair<hooker*, bool>>::iterator it(_hooks_begin);
       it != _hooks.end();)
    if (it->first == &h)
      it = _hooks.erase(it);
    else
      ++it;
  _hooks_begin = _hooks.begin();
  _hooks_end = _hooks.end();
}

/**
 *  Unload class instance.
 */
void engine::unload() {
  std::lock_guard<std::mutex> lk(_load_m);
  // Commit the cache file, if needed.
  if (_instance && _instance->_cache_file.get())
    _instance->_cache_file->commit();

  delete _instance;
  _instance = nullptr;
}

/**
 *  Unsubscribe from the multiplexing engine.
 *
 *  @param[in] subscriber  Subscriber.
 */
void engine::unsubscribe(muxer* subscriber) {
  std::lock_guard<std::mutex> lock(_muxers_m);
  for (auto it = _muxers.begin(), end = _muxers.end(); it != end; ++it)
    if (*it == subscriber) {
      _muxers.erase(it);
      break;
    }
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
engine::engine()
    : _hooks{},
      _hooks_begin{_hooks.begin()},
      _hooks_end{_hooks.end()},
      _engine_m{},
      _muxers{},
      _muxers_m{},
      _write_func(&engine::_nop) {}

/**
 *  Generate path to the multiplexing engine cache file.
 *
 *  @return Path to the multiplexing engine cache file.
 */
std::string engine::_cache_file_path() const {
  std::string retval(fmt::format("{}.unprocessed",
  config::applier::state::instance().cache_dir()));
  return retval;
}

/**
 *  Do nothing.
 *
 *  @param[in] d  Unused.
 */
void engine::_nop(std::shared_ptr<io::data> const& d) {
  (void)d;
}

/**
 *  Send queued events to subscribers.
 */
void engine::_send_to_subscribers() {
  // Process all queued events.
  std::lock_guard<std::mutex> lock(_muxers_m);
  while (!_kiew.empty()) {
    // Send object to every subscriber.
    for (muxer* m : _muxers)
      m->publish(_kiew.front());
    _kiew.pop();
  }
}

/**
 *  The real event publication is done here. This method is just called by
 *  the publish method. No need of a lock, it is already owned by the publish
 *  method.
 *
 *  @param[in] e  Data to publish.
 */
void engine::_write(std::shared_ptr<io::data> const& e) {
  // Send object to every hook.
  for (std::vector<std::pair<hooker*, bool>>::iterator it(_hooks_begin),
       end(_hooks_end);
       it != end; ++it)
    if (it->second) {
      it->first->write(e);
      std::shared_ptr<io::data> d;
      it->first->read(d);
      while (d) {
        _kiew.push(d);
        it->first->read(d);
      }
    }

  // Send events to subscribers.
  _send_to_subscribers();
}

/**
 *  Write to a cache file that will be played back at startup.
 *
 *  @param[in] d  Data to write.
 */
void engine::_write_to_cache_file(std::shared_ptr<io::data> const& d) {
  try {
    if (_cache_file)
      _cache_file->add(d);
  } catch (std::exception const& e) {
    logging::error(logging::medium)
        << "multiplexing: could not write to cache file: " << e.what();
  }
}
