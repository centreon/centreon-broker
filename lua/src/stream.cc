/*
** Copyright 2017 Centreon
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
#include <algorithm>
#include <cmath>
#include <sstream>
#include <sstream>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/lua/luabinding.hh"
#include "com/centreon/broker/lua/stream.hh"
#include "com/centreon/broker/misc/math.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::lua;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

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
      _next_stat{time(nullptr)},
      _nb_stats{0},
      _a_min{1},
      _exit{false},
      _flush{false} {
  bool fail = false;
  std::string fail_msg;

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

    try {
      lb = new luabinding(lua_script, conf_params, _cache);
      has_flush = lb->has_flush();
    }
    catch (std::exception const& e) {
      fail_msg = e.what();
      fail = true;
      _exit = true;
      return;
    }

    for (;;) {
      if (has_flush) {
        std::lock_guard<std::mutex> lck(_flush_m);
        if (_flush) {
          log_v2::lua()->debug("stream: flush event");
          int32_t res = lb->flush();
          {
            std::lock_guard<std::mutex> lock(_acks_count_m);
            log_v2::lua()->trace(
                "stream: {} events acknowledged by the script flush", res);
            _acks_count += res;
            log_v2::lua()->debug("stream: events to ack size: {}", _acks_count);
          }
          _flush = false;
        }
      }

      if (events.empty()) {
        std::lock_guard<std::mutex> lck(_exposed_events_m);
        std::swap(_exposed_events, _events);
      }

      if (!events.empty()) {
        std::shared_ptr<io::data> d = _events.front();
        _events.pop_front();
        uint32_t res = lb->write(d);
        log_v2::lua()->trace(
            "stream: {} events acknowledged by the script write", res);
        _acks_count += res;
        log_v2::lua()->debug("stream: events to ack size: {}", _acks_count);
      } else if (_exit) {
        /* We exit only if the events queue is empty */
        log_v2::lua()->debug("stream: exit");
        break;
      }
    }

    // No more need of the Lua interpreter
    delete lb;
  });

  if (fail) {
    _thread.join();
    throw exceptions::msg() << fail_msg;
  }
}

/**
 *  Destructor.
 */
stream::~stream() {
  _exit = true;
  if (_thread.joinable())
    _thread.join();
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
  throw exceptions::shutdown() << "cannot read from lua generic connector";
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


//  {
//    time_t now = time(nullptr);
//    if (now > _next_stat || std::isnan(_a)) {
//      *_stats_it = {now, _events.size()};
//      ++_nb_stats;
//      ++_stats_it;
//      if (_stats_it == _stats.end())
//        _stats_it = _stats.begin();
//
//      // We take a point at least every 30s.
//      bool res = misc::least_squares(_stats, _nb_stats, _a, _b);
//      if (!res) {
//        _a = NAN;
//        _b = NAN;
//      }
//      else {
//        _next_stat += 30;
//        if (_a > _a_min) {
//          _a_min = _a;
//          logging::debug(logging::high) << "LUA: The streamconnector looks "
//            "quite slow, waiting events are increasing at the speed of "
//            << _a << "events/s";
//        }
//      }
//    }
//  }

  int retval = _acks_count;
  _acks_count -= retval;
  log_v2::lua()->debug("stream: {} events will be acknowledged at the end of the write function", retval);
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
  {
    std::lock_guard<std::mutex> lck(_flush_m);
    log_v2::lua()->debug("stream: flush forced");
    _flush = true;
  }

  int retval = _acks_count;
  _acks_count -= retval;
  log_v2::lua()->debug("stream: flush {} events acknowledged", retval);
  return retval;
}

void stream::statistics(json11::Json::object& tree) const {
  tree["waiting_events"] =
      json11::Json::object{{"lm", json11::Json::object{{"a", _a}, {"b", _b}}},
                           {"total", static_cast<double>(_events.size())}};
}
