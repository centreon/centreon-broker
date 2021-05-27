/*
** Copyright 2017-2021 Centreon
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
#include "com/centreon/broker/lua/stream.hh"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/lua/luabinding.hh"
#include "com/centreon/broker/misc/math.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::exceptions;
using namespace com::centreon::broker::lua;

/**
 *  Constructor.
 *
 *  @param[in] addr                    Address to connect to
 *  @param[in] port                    port
 */
stream::stream(std::string const& lua_script,
               std::map<std::string, misc::variant> const& conf_params,
               std::shared_ptr<persistent_cache> const& cache)
    : io::stream("lua"),
      _cache{cache},
      _acks_count{0},
      _stats{{}},
      _stats_it{_stats.begin()},
      _next_stat{std::chrono::system_clock::now()},
      _exit{false},
      _flush{false} {
  bool fail = false;
  std::string fail_msg;
  std::mutex init_m;
  std::condition_variable init_cv;
  std::unique_lock<std::mutex> lock(init_m);
  bool configured = false;

  /* The lua interpreter does not support exchanges with several threads from
   * the outside. By design, the filter is called from another thread than the
   * one used by the write function.
   * To fulfill this difficulty, the filter and write functions feed this thread
   * with their arguments. The filter waits for an answer whereas the write
   * function just increases an _acks_count to inform broker on treated events.
   */
  _thread = std::thread([&] {
    // Access to the Lua interpreter
    luabinding* lb = nullptr;
    bool has_flush = false;
    std::deque<std::shared_ptr<io::data>> events;

    // Access to external variables, encapsulated because the thread will
    // continue whereas the output function will be over.
    {
      std::unique_lock<std::mutex> lock(init_m);

      try {
        lb = new luabinding(lua_script, conf_params, _cache);
        has_flush = lb->has_flush();
      } catch (std::exception const& e) {
        fail_msg = e.what();
        fail = true;
        _exit = true;
      }

      configured = true;
      init_cv.notify_all();
      if (fail)
        return;
    }

    /**
     * Events handling starts really here. */

    /* The count of events handled each second. */
    int count = 0;
    log_v2::lua()->debug("lua: starting internal thread.");
    for (;;) {
      /* If the thread queue is empty, we swap it with exposed events */
      if (events.empty()) {
        std::lock_guard<std::mutex> lck(_exposed_events_m);
        std::swap(_exposed_events, events);
        _events_size = events.size();
      }

      /* Every seconds, we store how many events have been handled. We can
       * then build an array of 10 values used to compute a speed. */
      {
        auto now = std::chrono::system_clock::now();
        if (now > _next_stat) {
          *_stats_it = count;
          count = 0;
          ++_stats_it;
          if (_stats_it == _stats.end())
            _stats_it = _stats.begin();

          // We take a point at least every 1s.
          _next_stat =
              std::chrono::system_clock::now() + std::chrono::seconds(1);
        }
      }

      /* Are there events to handle? */
      if (!events.empty()) {
        std::shared_ptr<io::data> d = events.front();
        events.pop_front();
        --_events_size;
        ++count;
        uint32_t res = lb->write(d);
        log_v2::lua()->trace(
            "stream: {} events acknowledged by the script write", res);
        _acks_count += res;
        log_v2::lua()->debug("stream: events to ack size: {}", _acks_count);
      } else if (_exit) {
        /* We exit only if the events queue is empty */
        log_v2::lua()->debug("stream: exit");
        break;
      } else {
        auto next_time =
            std::chrono::system_clock::now() + std::chrono::milliseconds(50);
        /* Is the flush activated and have we received a flush ? */
        if (has_flush && _flush) {
          log_v2::lua()->debug("stream: flush event");
          int32_t res = lb->flush();
          log_v2::lua()->trace(
              "stream: {} events acknowledged by the script flush", res);
          _acks_count += res;
          log_v2::lua()->debug("stream: events to ack size: {}", _acks_count);
          _flush = false;
        }

        /* We did nothing, let's wait for 50ms. We don't cook an egg with our
         * cpus. */
        std::this_thread::sleep_until(next_time);
      }
    }
    /* Is the flush activated? Then we flush a last time. */
    if (has_flush) {
      log_v2::lua()->debug("stream: flush event");
      int32_t res = lb->flush();
      log_v2::lua()->trace(
          "stream: {} acknowledged events by the script flush()", res);
      _acks_count += res;
      log_v2::lua()->debug("stream: events to ack size: {}", _acks_count);
    }

    // No more need of the Lua interpreter
    delete lb;
  });

  init_cv.wait(lock, [&configured] { return configured; });
  if (fail) {
    _thread.join();
    throw msg_fmt(fail_msg);
  }
}

/**
 *  Destructor.
 */
stream::~stream() {
  log_v2::lua()->debug("Destruction of Lua stream");
  assert(_exit);
}

/**
 * @brief Stops the stream and flushes data.
 *
 * @return The number of acknowledged events.
 */
int32_t stream::stop() {
  _exit = true;
  if (_thread.joinable())
    _thread.join();
  int32_t retval = _acks_count;
  _acks_count = 0;
  log_v2::core()->info("lua stream stopped with {} acknowledged events",
                       retval);
  return retval;
}

/**
 *  Read from the connector.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw exceptions::shutdown("cannot read from lua generic connector");
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& data) {
  assert(data);
  {
    std::lock_guard<std::mutex> lck(_exposed_events_m);
    _exposed_events.push_back(data);
  }

  int retval = _acks_count;
  _acks_count -= retval;
  log_v2::lua()->debug(
      "stream: {} events will be acknowledged at the end of the write function",
      retval);
  return retval;
}

/**
 *  Events have been transmitted to the Lua connector, several of them have
 *  been treated and can now be acknowledged by broker. This function returns
 *  how many are in that case.
 *
 * @return The number of events to ack.
 */
int stream::flush() {
  if (!_flush) {
    log_v2::lua()->debug("stream: flush forced");
    _flush = true;
  }

  int retval = _acks_count;
  _acks_count -= retval;
  log_v2::lua()->debug("stream: flush {} events acknowledged", retval);
  return retval;
}

void stream::statistics(nlohmann::json& tree) const {
  std::lock_guard<std::mutex> lck(_exposed_events_m);
  double avg = 0;
  for (auto& s : _stats)
    avg += s;
  avg /= _stats.size();
  tree["waiting_events"] = nlohmann::json{
      {"speed", fmt::format("{} events/s", avg)},
      {"handled_events", static_cast<int>(_events_size)},
      {"waiting_events", static_cast<int>(_exposed_events.size())}};
}
