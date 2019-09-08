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

#include <memory>
#include <mutex>
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
class engine : public std::recursive_mutex {
 public:
  ~engine();
  void clear();
  void hook(hooker& h, bool with_data = true);
  static engine& instance();
  static void load();
  void publish(std::shared_ptr<io::data> const& d);
  void start();
  void stop();
  void subscribe(muxer* subscriber);
  void unhook(hooker& h);
  static void unload();
  void unsubscribe(muxer* subscriber);

 private:
  engine();
  engine(engine const& other);
  engine& operator=(engine const& other);
  std::string _cache_file_path() const;
  void _nop(std::shared_ptr<io::data> const& d);
  void _send_to_subscribers();
  void _write(std::shared_ptr<io::data> const& d);
  void _write_to_cache_file(std::shared_ptr<io::data> const& d);

  static engine* _instance;
  void (engine::*_write_func)(std::shared_ptr<io::data> const&);
  std::unique_ptr<persistent_cache> _cache_file;
};
}  // namespace multiplexing

CCB_END()

#endif  // !CCB_MULTIPLEXING_ENGINE_HH
