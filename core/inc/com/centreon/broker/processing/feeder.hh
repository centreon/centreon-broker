/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_PROCESSING_FEEDER_HH
#define CCB_PROCESSING_FEEDER_HH

#include <atomic>
#include <climits>
#include <condition_variable>
#include <memory>
#include <string>
#include <thread>

#include "com/centreon/broker/misc/shared_mutex.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/processing/stat_visitable.hh"

CCB_BEGIN()

// Forward declaration.
namespace io {
class stream;
}

namespace processing {
/**
 *  @class feeder feeder.hh "com/centreon/broker/processing/feeder.hh"
 *  @brief Feed events from a source to a destination.
 *
 *  Take events from a source and send them to a destination.
 */
class feeder : public stat_visitable {
  enum state { stopped, running, finished };
  // Condition variable used when waiting for the thread to finish
  std::thread _thread;
  state _state;
  mutable std::mutex _state_m;
  std::condition_variable _state_cv;

  std::atomic_bool _should_exit;

  std::unique_ptr<io::stream> _client;
  multiplexing::subscriber _subscriber;

  // This mutex is used for the stat thread.
  mutable misc::shared_mutex _client_m;

  void _callback() noexcept;

 protected:
  const std::string& _get_read_filters() const override;
  const std::string& _get_write_filters() const override;
  void _forward_statistic(json11::Json::object& tree) override;
  uint32_t _get_queued_events() const override;

 public:
  feeder(const std::string& name,
         std::unique_ptr<io::stream>& client,
         const std::unordered_set<uint32_t>& read_filters,
         const std::unordered_set<uint32_t>& write_filters);
  ~feeder();
  feeder(const feeder&) = delete;
  feeder& operator=(const feeder&) = delete;
  bool is_finished() const noexcept;
  const char* get_state() const;
};
}  // namespace processing

CCB_END()

#endif  // !CCB_PROCESSING_FEEDER_HH
