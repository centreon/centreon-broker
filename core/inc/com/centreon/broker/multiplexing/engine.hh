/*
** Copyright 2009-2012,2015,2019-2021 Centreon
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
 *  Core multiplexing engine. Sends events to and receives events from
 *  muxer objects.
 *
 *  This class has a unique instance. Before calling the instance() method,
 *  we have to call the static load() one. And to close this instance, we
 *  have to call the static method unload().
 *
 *  The instance initialization/deinitialization are guarded by a mutex
 *  _load_m. It is only used for that purpose.
 *
 *  This class is the root of events dispatching. Events arrive from a stream
 *  are transfered to a muxer and then to engine (at the root of the tree).
 *  This one then sends the event to all its children. Each muxer receives
 *  the event and sends it to its stream.
 *
 *  The engine has three states:
 *  * switched off, the 'write' function points to a _nop() function. All event
 *    that could be received is lost by the engine. This state is possible only
 *    when the engine is started or during tests.
 *  * running, the 'write' function points to a _write() function that sends
 *    received events to all the muxers beside.
 *  * stopped, the 'write' function points to a _write_to_cache_file() funtion.
 *    When broker is stopped, before it to be totally stopped, events are
 *    written to a cache file ...unprocessed... This file will be re-read at the
 *    next broker start.
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

  static std::mutex _load_m;

  engine();
  std::string _cache_file_path() const;
  void _nop(std::shared_ptr<io::data> const& d);
  void _send_to_subscribers();
  void _write(std::shared_ptr<io::data> const& d);
  void _write_to_cache_file(std::shared_ptr<io::data> const& d);
  void _publish(std::shared_ptr<io::data> const& d);

  void (engine::*_write_func)(std::shared_ptr<io::data> const&);

 public:
  static void load();
  static void unload();
  static engine& instance();

  engine(engine const&) = delete;
  engine& operator=(engine const&) = delete;
  ~engine() noexcept = default;
  void clear();
  void publish(const std::shared_ptr<io::data>& d);
  void publish(const std::list<std::shared_ptr<io::data>>& to_publish);
  void start();
  void stop();
  void hook(hooker& h, bool with_data = true);
  void unhook(hooker& h);
  void subscribe(muxer* subscriber);
  void unsubscribe(muxer* subscriber);
};
}  // namespace multiplexing

CCB_END()

#endif  // !CCB_MULTIPLEXING_ENGINE_HH
