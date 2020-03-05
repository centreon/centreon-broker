/*
** Copyright 2011-2017 Centreon
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

#ifndef CCB_STORAGE_STREAM_HH
#define CCB_STORAGE_STREAM_HH

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/namespace.hh"
//#include "com/centreon/broker/storage/rebuilder.hh"

CCB_BEGIN()

// Forward declaration.
class database_config;

namespace storage {
/**
 *  @class stream stream.hh "com/centreon/broker/storage/stream.hh"
 *  @brief Storage stream.
 *
 *  Handle perfdata and insert proper informations in index_data and
 *  metrics table of a centstorage DB.
 */
class stream : public io::stream {
  struct index_info {
    std::string host_name;
    uint32_t index_id;
    bool locked;
    uint32_t rrd_retention;
    std::string service_description;
    bool special;
  };
  struct metric_info {
    bool locked;
    uint32_t metric_id;
    uint32_t type;
    double value;
    std::string unit_name;
    double warn;
    double warn_low;
    bool warn_mode;
    double crit;
    double crit_low;
    bool crit_mode;
    double min;
    double max;
  };
  struct metric_value {
    time_t c_time;
    uint32_t metric_id;
    short status;
    double value;
  };

  int32_t _pending_events;
  //rebuilder _rebuilder;
  std::string _status;
  mutable std::mutex _statusm;

  void _update_status(std::string const& status);
 public:
  stream(uint32_t rrd_len,
         uint32_t interval_length,
         uint32_t rebuild_check_interval,
         bool store_in_db = true);
  stream(stream const&) = delete;
  stream& operator=(stream const&) = delete;
  ~stream();
  int32_t flush();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void statistics(json11::Json::object& tree) const;
  int32_t write(std::shared_ptr<io::data> const& d);
};
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_STREAM_HH
