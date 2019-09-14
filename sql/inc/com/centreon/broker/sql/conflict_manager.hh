/*
** Copyright 2019 Centreon
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
#ifndef CCB_SQL_CONFLICT_MANAGER_HH
#define CCB_SQL_CONFLICT_MANAGER_HH
#include <array>
#include <condition_variable>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/mysql.hh"

CCB_BEGIN()
namespace sql {
class conflict_manager {
  static conflict_manager* _singleton;
  static std::mutex _init_m;
  static std::condition_variable _init_cv;

  /* When events arrive in the conflict_manager, two things are done.
   * A boolean is inserted at the end of the queue with the value false.
   * A pair is made with the event and a pointer to the boolean.
   * This pair is inserted following the event category in the array.
   * In each case of the array we have a list containing events of the same
   * type.
   * The idea behind all of this is we can treat events by type. When an event
   * is done, we have access to the boolean stored in the queue to set it to
   * true.
   * And later, the stream that sent events will know how many events can be
   * released
   * from the retention queue. */
  std::array<std::list<std::pair<std::shared_ptr<io::data>, bool*> >, 27>
      _neb_events;

  /* Since the sql and storage streams use this conflict_manager, we must
   * manage two queues, the first for sql and the second one for storage.
   * So they will know when their events will be released. */
  std::array<std::deque<bool>, 2> _queue;

  mutable std::mutex _loop_m;
  std::condition_variable _loop_cv;
  bool _exit;
  uint32_t _max_pending_queries;
  uint32_t _pending_queries;
  mysql _mysql;

  std::thread _thread;

  std::unordered_map<unsigned int, unsigned int> _cache_host_instance;

  database::mysql_stmt _instance_insupdate;
  database::mysql_stmt _host_insupdate;

  conflict_manager(database_config const& dbcfg);
  conflict_manager() = delete;
  conflict_manager& operator=(conflict_manager const& other) = delete;
  conflict_manager(conflict_manager const& other) = delete;
  bool _should_exit() const;
  void _callback();
  void _set_booleans(
      std::list<std::pair<std::shared_ptr<io::data>, bool*> >& lst);

  void _process_instances();
  void _process_hosts();
  void _clean_tables(uint32_t instance_id);
  bool _is_valid_poller(uint32_t instance_id);

 public:
  enum stream_type {
    sql,
    storage
  };

  static void init(database_config const& dbcfg);
  static bool wait_for_init();
  static conflict_manager& instance();

  void send_event(stream_type c, std::shared_ptr<io::data> const& e);
  int32_t get_acks(stream_type c);
};
}  // namespace sql
CCB_END()

#endif /* !CCB_SQL_CONFLICT_MANAGER_HH */
