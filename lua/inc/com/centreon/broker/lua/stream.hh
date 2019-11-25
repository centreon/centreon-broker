/*
** Copyright 2018 Centreon
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

#ifndef CCB_LUA_STREAM_HH
#define CCB_LUA_STREAM_HH

#include <deque>
#include <atomic>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/variant.hh"

CCB_BEGIN()

namespace lua {

/**
 *  @class stream stream.hh "com/centreon/broker/lua/stream.hh"
 *  @brief lua stream.
 *
 *  Stream events into lua database.
 */
class stream : public io::stream {

  std::thread _thread;

  /* Macro cache */
  macro_cache _cache;

  /* Management of the main loop */
  std::mutex _loop_m;
  std::condition_variable _loop_cv;

  /* The write stuff */
  std::deque<std::shared_ptr<io::data>> _events;
  std::mutex _acks_count_m;
  uint32_t _acks_count;

  /* The filter stuff */
  uint32_t _filter;                   // input
  bool _filter_ok;                    // output
  std::mutex _filter_m;               // mutex used to wait for output
  std::condition_variable _filter_cv; // cv to wait for output

  /* The exit flag */
  bool _exit;

 public:
  stream(std::string const& lua_script,
         std::map<std::string, misc::variant> const& conf_params,
         std::shared_ptr<persistent_cache> const& cache);
  stream& operator=(stream const& other) = delete;
  stream(stream const& other) = delete;
  ~stream();
  bool read(std::shared_ptr<io::data>& d, time_t deadline) override;
  int write(std::shared_ptr<io::data> const& d) override;
  bool filter(uint32_t type);
};
}  // namespace lua

CCB_END()

#endif  // !CCB_LUA_STREAM_HH
