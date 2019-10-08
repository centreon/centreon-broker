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
#  define CCB_STORAGE_STREAM_HH

#  include <deque>
#  include <list>
#  include <mutex>
#  include <map>
#  include <memory>
#  include <QSqlDatabase>
#  include <QString>
#  include <utility>
#  include "com/centreon/broker/database.hh"
#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/rebuilder.hh"

CCB_BEGIN()

// Forward declaration.
class              database_config;

namespace          storage {
  /**
   *  @class stream stream.hh "com/centreon/broker/storage/stream.hh"
   *  @brief Storage stream.
   *
   *  Handle perfdata and insert proper informations in index_data and
   *  metrics table of a centstorage DB.
   */
  class            stream : public io::stream {
   public:
                   stream(
                     database_config const& db_cfg,
                     unsigned int rrd_len,
                     unsigned int interval_length,
                     unsigned int rebuild_check_interval,
                     bool store_in_db = true,
                     bool insert_in_index_data = false);
                   ~stream();
    int            flush();
    bool           read(std::shared_ptr<io::data>& d, time_t deadline);
    void           statistics(io::properties& tree) const;
    void           update();
    int            write(std::shared_ptr<io::data> const& d);

   private:
    struct         index_info {
      QString      host_name;
      unsigned int index_id;
      bool         locked;
      unsigned int rrd_retention;
      QString      service_description;
      bool         special;
    };
    struct         metric_info {
      bool         locked;
      unsigned int metric_id;
      unsigned int type;
      double       value;
      QString      unit_name;
      double       warn;
      double       warn_low;
      bool         warn_mode;
      double       crit;
      double       crit_low;
      bool         crit_mode;
      double       min;
      double       max;
    };
    struct         metric_value {
      time_t       c_time;
      unsigned int metric_id;
      short        status;
      double       value;
    };

                   stream(stream const& other);
    stream&        operator=(stream const& other);
    void           _check_deleted_index();
    void           _delete_metrics(
                     std::list<unsigned long long> const& metrics_to_delete);
    unsigned int   _find_index_id(
                     unsigned int host_id,
                     unsigned int service_id,
                     QString const& host_name,
                     QString const& service_desc,
                     unsigned int* rrd_len,
                     bool* locked);
    unsigned int   _find_metric_id(
                     unsigned int index_id,
                     QString metric_name,
		     QString const& unit_name,
		     double warn,
                     double warn_low,
                     bool warn_mode,
		     double crit,
                     double crit_low,
                     bool crit_mode,
		     double min,
		     double max,
                     double value,
                     unsigned int* type,
                     bool* locked);
    void           _insert_perfdatas();
    void           _prepare();
    void           _rebuild_cache();
    void           _update_status(std::string const& status);

    std::map<std::pair<unsigned int, unsigned int>, index_info>
                   _index_cache;
    bool           _insert_in_index_data;
    unsigned int   _interval_length;
    std::map<std::pair<unsigned int, QString>, metric_info>
                   _metric_cache;
    unsigned int   _pending_events;
    std::deque<metric_value>
                   _perfdata_queue;
    rebuilder      _rebuild_thread;
    unsigned int   _rrd_len;
    std::string    _status;
    mutable std::mutex
                   _statusm;
    bool           _store_in_db;
    database       _db;
    database_query _data_bin_insert;
    database_query _update_metrics;
    database_query _index_data_insert;
    database_query _index_data_update;
  };
}

CCB_END()

#endif // !CCB_STORAGE_STREAM_HH
