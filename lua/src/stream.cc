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
#include <sstream>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/lua/luabinding.hh"
#include "com/centreon/broker/lua/stream.hh"
//#include "com/centreon/broker/multiplexing/muxer.hh"

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
    : _cache{cache}, _acks_count{0}, _filter{0}, _exit{false} {


  /* The lua interpreter does not support exchanges with several threads from
   * the outside. By design, the filter is called from another thread than the
   * one used for the write function.
   * To fulfill this difficulty, the filter and write functions feed this thread
   * with their arguments. The filter waits for an answer whereas the write
   * function just increases an _acks_count to inform broker on treated events.
   */
  _thread = std::thread([&] {
    // Access to the Lua interpreter
    luabinding* lb = new luabinding(lua_script, conf_params, _cache);

    // If there is a filter, register it.
//    if (lb->has_filter()) {
//      std::function<bool(uint32_t)> func =
//          std::bind(&stream::filter, this, std::placeholders::_1);
//      multiplexing::muxer::register_read_filter(func);
//    }

    std::unique_lock<std::mutex> lock(_loop_m);
    for (;;) {
      // count increases each time something is asked to be done
      _loop_cv.wait(lock, [this] { return _exit || _filter || !_events.empty(); });

      // No more commands are waiting, we can exit.
      if (!_filter && _events.empty())
        break;

      // Before working on events, we consider filter in priority.
      if (_filter) {
        lock.unlock();
        {
          std::lock_guard<std::mutex> ans_lock(_filter_m);
          _filter_ok = lb->filter(_filter);
          _filter = 0;
          _filter_cv.notify_one();
        }
        lock.lock();
      }

      if (!_events.empty()) {
        std::shared_ptr<io::data>& d = _events.front();
        _events.pop_front();
        lock.unlock();
        uint32_t res = lb->write(d);
        {
          std::lock_guard<std::mutex> lock(_acks_count_m);
          _acks_count += res;
        }
        lock.lock();
      }
    }

//    // Unregister the filter
//    multiplexing::muxer::unregister_read_filter();

    // No more need of the Lua interpreter
    delete lb;
  });
}

/**
 *  Destructor.
 */
stream::~stream() {
  {
    std::lock_guard<std::mutex> lock(_loop_m);
    _exit = true;
    _loop_cv.notify_one();
  }
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
  throw(exceptions::shutdown() << "cannot read from lua generic connector");
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& data) {
  if (!validate(data, "lua"))
    return 0;

  {
    std::lock_guard<std::mutex> lock(_loop_m);
    _events.push_back(data);
  }
  _loop_cv.notify_one();

  {
    std::lock_guard<std::mutex> lock(_acks_count_m);
    int retval = _acks_count;
    _acks_count = 0;
    return retval;
  }
}

bool stream::filter(uint32_t type) {
  // Ask for a type
  std::unique_lock<std::mutex> loop_lock(_loop_m);
  _filter = type;
  _loop_cv.notify_one();
  loop_lock.unlock();

  // Wait for the answer
  std::unique_lock<std::mutex> ans_lock(_filter_m);
  _filter_cv.wait(ans_lock, [this] { return _filter == 0; });
  return _filter_ok;
}
