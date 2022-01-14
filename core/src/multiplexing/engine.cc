/*
** Copyright 2009-2013,2015, 2020-2021 Centreon
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

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <vector>

#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/pool.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

// Class instance.
engine* engine::_instance{nullptr};
std::mutex engine::_load_m;

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
  log_v2::core()->trace("multiplexing: loading engine");
  std::lock_guard<std::mutex> lk(_load_m);
  if (!_instance)
    _instance = new engine;
}

/**
 *  Unload class instance.
 */
void engine::unload() {
  if (!_instance)
    return;
  if (_instance->_state != stopped)
    _instance->stop();

  log_v2::core()->trace("multiplexing: unloading engine");
  std::lock_guard<std::mutex> lk(_load_m);
  // Commit the cache file, if needed.
  if (_instance && _instance->_cache_file)
    _instance->_cache_file->commit();

  delete _instance;
  _instance = nullptr;
}

/**
 *  Send an event to all subscribers.
 *
 *  @param[in] e  Event to publish.
 */
void engine::publish(const std::shared_ptr<io::data>& e) {
  // Lock mutex.
  std::lock_guard<std::mutex> lock(_engine_m);
  switch (_state) {
    case stopped:
      _cache_file->add(e);
      _unprocessed_events++;
      break;
    case not_started:
      _kiew.push_back(e);
      break;
    default:
      _kiew.push_back(e);
      if (!_sending_to_subscribers) {
        _sending_to_subscribers = true;
        _strand.post(std::bind(&engine::_send_to_subscribers, this));
      }
      break;
  }
}

void engine::publish(const std::list<std::shared_ptr<io::data>>& to_publish) {
  std::lock_guard<std::mutex> lock(_engine_m);
  switch (_state) {
    case stopped:
      for (auto& e : to_publish) {
        _cache_file->add(e);
        _unprocessed_events++;
      }
      break;
    case not_started:
      for (auto& e : to_publish)
        _kiew.push_back(e);
      break;
    default:
      for (auto& e : to_publish)
        _kiew.push_back(e);
      if (!_sending_to_subscribers) {
        _sending_to_subscribers = true;
        _strand.post(std::bind(&engine::_send_to_subscribers, this));
      }
      break;
  }
}

/**
 *  Start multiplexing. This function gets back the retention content and
 *  inserts it in front of the engine's queue. Then all this content is
 *  published.
 */
void engine::start() {
  std::lock_guard<std::mutex> lock(_engine_m);
  if (_state == not_started) {
    // Set writing method.
    log_v2::core()->debug("multiplexing: engine starting");
    _state = running;

    // Local queue.
    std::deque<std::shared_ptr<io::data>> kiew;
    // Get events from the cache file to the local queue.
    try {
      persistent_cache cache(_cache_file_path());
      std::shared_ptr<io::data> d;
      for (;;) {
        cache.get(d);
        if (!d)
          break;
        kiew.push_back(d);
      }
    } catch (const std::exception& e) {
      log_v2::core()->error("multiplexing: engine couldn't read cache file: {}",
                            e.what());
    }

    // Copy global event queue to local queue.
    while (!_kiew.empty()) {
      kiew.push_back(_kiew.front());
      _kiew.pop_front();
    }

    // Send events queued while multiplexing was stopped.
    _kiew = std::move(kiew);
    if (!_sending_to_subscribers) {
      _sending_to_subscribers = true;
      _strand.post(std::bind(&engine::_send_to_subscribers, this));
    }
  }
  log_v2::core()->info("multiplexing: engine started");
}

/**
 *  Stop multiplexing.
 */
void engine::stop() {
  std::unique_lock<std::mutex> lock(_engine_m);
  if (_state != stopped) {
    // Notify hooks of multiplexing loop end.
    log_v2::core()->info("multiplexing: stopping engine");

    do {
      // Make sure that no more data is available.
      if (!_sending_to_subscribers) {
        log_v2::core()->info(
            "multiplexing: sending events to muxers for the last time");
        _sending_to_subscribers = true;
        lock.unlock();
        std::promise<void> promise;
        _strand.post([this, &promise] {
          _send_to_subscribers();
          promise.set_value();
        });
        promise.get_future().get();
        lock.lock();
      }
    } while (!_kiew.empty());

    // Open the cache file and start the transaction.
    // The cache file is used to cache all the events produced
    // while the engine is stopped. It will be replayed next time
    // the engine is started.
    try {
      _cache_file.reset(new persistent_cache(_cache_file_path()));
      _cache_file->transaction();
    } catch (const std::exception& e) {
      log_v2::perfdata()->error("multiplexing: could not open cache file: {}",
                                e.what());
      _cache_file.reset();
    }

    // Set writing method.
    _state = stopped;
  }
  log_v2::core()->debug("multiplexing: engine stopped");
}

/**
 *  Subscribe to the multiplexing engine.
 *
 *  @param[in] subscriber  Subscriber.
 */
void engine::subscribe(muxer* subscriber) {
  std::promise<void> promise;
  _strand.post([this, &promise, subscriber] {
    log_v2::core()->trace("muxer {} subscribes to engine", subscriber->name());
    _muxers.push_back(subscriber);
    promise.set_value();
  });
  promise.get_future().get();
}

/**
 *  Unsubscribe from the multiplexing engine.
 *
 *  @param[in] subscriber  Subscriber.
 */
void engine::unsubscribe(muxer* subscriber) {
  std::promise<void> promise;
  _strand.post([this, &promise, subscriber] {
    for (auto it = _muxers.begin(), end = _muxers.end(); it != end; ++it)
      if (*it == subscriber) {
        log_v2::core()->trace("muxer {} unsubscribes to engine",
                              subscriber->name());
        _muxers.erase(it);
        break;
      }
    promise.set_value();
  });
  promise.get_future().get();
}

/**
 *  Default constructor.
 */
engine::engine()
    : _state{not_started},
      _strand(pool::instance().io_context()),
      _muxers{},
      _unprocessed_events{0u},
      _sending_to_subscribers{false} {}

/**
 *  Generate path to the multiplexing engine cache file.
 *
 *  @return Path to the multiplexing engine cache file.
 */
std::string engine::_cache_file_path() const {
  std::string retval(fmt::format(
      "{}.unprocessed", config::applier::state::instance().cache_dir()));
  return retval;
}

/**
 *  Send queued events to subscribers. Since events are queued, we use a strand
 *  to keep their order. But there are several muxers, so we parallelize the
 *  sending of data to each.
 */
void engine::_send_to_subscribers() {
  // Process all queued events.
  std::deque<std::shared_ptr<io::data>> kiew;
  {
    std::lock_guard<std::mutex> lck(_engine_m);
    if (_kiew.empty()) {
      _sending_to_subscribers = false;
      return;
    }
    std::swap(_kiew, kiew);
  }

  std::atomic<size_t> count{_muxers.size()};
  if (count > 0) {
    /* We must wait the end of the sending, so we use a promise. */
    std::promise<void> promise;

    /* Since the sending is parallelized, we use the thread pool for this
     * purpose except for the last muxer where we use this thread. */

    /* We get an iterator to the last muxer */
    auto it_last = --_muxers.end();

    /* We use the thread pool for the muxers from the first one to the second
     * to last */
    for (auto it = _muxers.begin(); it != it_last; ++it) {
      pool::io_context().post([&kiew, m = *it, &count, &promise] {
        for (auto& e : kiew)
          m->publish(e);
        --count;
        if (count == 0)
          promise.set_value();
      });
    }
    /* The same work but by this thread for the last muxer. */
    auto m = *it_last;
    for (auto& e : kiew)
      m->publish(e);
    --count;

    if (count == 0)
      promise.set_value();

    promise.get_future().get();
  }

  /* The strand is necessary for the order of data */
  _strand.post(std::bind(&engine::_send_to_subscribers, this));
}

/**
 *  Clear events stored in the multiplexing engine.
 */
void engine::clear() {
  _kiew.clear();
}
