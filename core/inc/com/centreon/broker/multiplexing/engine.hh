/*
** Copyright 2009-2012,2015 Centreon
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

#ifndef CCB_MULTIPLEXING_ENGINE_HH
#define CCB_MULTIPLEXING_ENGINE_HH

#include <list>
#include <memory>
#include <mutex>
#include <queue>

#include "com/centreon/broker/multiplexing/hooker.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace multiplexing {
// Forward declaration.
class muxer;

/**
 *  @class engine engine.hh "com/centreon/broker/multiplexing/engine.hh"
 *  @brief Multiplexing engine.
 *
 *  Core multiplexing engine. Send events to and receive events from
 *  muxer objects.
 *
 *  @see muxer
 */
class engine {
  static engine* _instance;
  std::unique_ptr<persistent_cache> _cache_file;

  // Data queue.
  std::queue<std::shared_ptr<io::data>> _kiew;

  // Hooks
  std::vector<std::pair<hooker*, bool>> _hooks;
  std::vector<std::pair<hooker*, bool>>::iterator _hooks_begin;
  std::vector<std::pair<hooker*, bool>>::iterator _hooks_end;

  // Mutex to lock _kiew and _hooks
  std::mutex _engine_m;

  // Subscriber.
  std::vector<muxer*> _muxers;
  std::mutex _muxers_m;

  engine();
  std::string _cache_file_path() const;
  void _nop(std::shared_ptr<io::data> const& d);
  void _send_to_subscribers();
  void _write(std::shared_ptr<io::data> const& d);
  void _write_to_cache_file(std::shared_ptr<io::data> const& d);
  void _publish(std::shared_ptr<io::data> const& d);

  void (engine::*_write_func)(std::shared_ptr<io::data> const&);

 public:
  engine(engine const& other) = delete;
  engine& operator=(engine const& other) = delete;
  ~engine();
  void clear();
  void hook(hooker& h, bool with_data = true);
  static engine& instance();
  static void load();
  static std::mutex _load_m;
  void publish(std::shared_ptr<io::data> const& d);
  void publish(std::list<std::shared_ptr<io::data>> const& to_publish);
  void start();
  void stop();
  void subscribe(muxer* subscriber);
  void unhook(hooker& h);
  static void unload();
  void unsubscribe(muxer* subscriber);
};
}  // namespace multiplexing

CCB_END()

#endif  // !CCB_MULTIPLEXING_ENGINE_HH
