/*
** Copyright 2015 Centreon
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

#ifndef CCB_NEB_NODE_EVENTS_STREAM_HH
#define CCB_NEB_NODE_EVENTS_STREAM_HH

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/downtime_map.hh"
#include "com/centreon/broker/neb/downtime_scheduler.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/node_cache.hh"
#include "com/centreon/broker/neb/node_id.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class node_events_stream node_events_stream.hh
 * "com/centreon/broker/neb/node_events_stream.hh"
 *  @brief Node events stream.
 *
 *  Manage node events: downtime, acks, etc.
 */
class node_events_stream : public io::stream {
 public:
  node_events_stream(std::string const& name,
                     std::shared_ptr<persistent_cache> cache,
                     std::string const& config_file);
  ~node_events_stream();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void update();
  int write(std::shared_ptr<io::data> const& d);
  void parse_command(extcmd::command_request const& exc, io::stream& stream);
  void set_timeperiods(
      std::unordered_map<std::string, time::timeperiod::ptr> const& tps);

 private:
  node_events_stream(node_events_stream const& other);
  node_events_stream& operator=(node_events_stream const& other);

  std::shared_ptr<persistent_cache> _cache;
  std::string _config_file;
  std::string _name;

  // Timeperiods.
  std::unordered_map<std::string, time::timeperiod::ptr> _timeperiods;

  // Host/Service caches.
  node_cache _node_cache;

  void _process_host_status(neb::host_status const& hst);
  void _process_service_status(neb::service_status const& sst);
  void _update_downtime(neb::downtime const& dwn);
  void _remove_expired_acknowledgement(node_id node,
                                       timestamp check_time,
                                       short prev_state,
                                       short state);
  void _trigger_floating_downtime(node_id node, short state);

  // Acks and downtimes caches.
  std::unordered_map<node_id, neb::acknowledgement> _acknowledgements;
  std::vector<neb::downtime> _incomplete_downtime;
  downtime_map _downtimes;
  downtime_scheduler _downtime_scheduler;

  // Parsing methods.
  enum ack_type { ack_host = 0, ack_service };

  enum down_type { down_service = 1, down_host = 2, down_host_service = 3 };
  void _parse_ack(ack_type type, const char* args, io::stream& stream);
  void _parse_remove_ack(ack_type type, const char* args, io::stream& stream);
  void _parse_downtime(down_type type, const char* args, io::stream& stream);
  void _parse_remove_downtime(down_type type,
                              const char* args,
                              io::stream& stream);
  void _schedule_downtime(downtime const& dwn);
  void _spawn_recurring_downtime(timestamp when, downtime const& dwn);

  // Downtime utility methods.
  void _register_downtime(downtime const& d, io::stream* stream);
  void _delete_downtime(downtime const& d, timestamp ts, io::stream* stream);

  // Cache and config loading.
  void _check_downtime_timeperiod_consistency();
  void _load_config_file();
  void _load_cache();
  void _process_loaded_event(std::shared_ptr<io::data> const& d);
  void _apply_config_downtimes();
  void _save_cache();
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_NODE_EVENTS_STREAM_HH
