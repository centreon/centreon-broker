/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_NOTIFICATION_STREAM_HH
#define CCB_NOTIFICATION_STREAM_HH

#include <QPair>
#include <QSet>
#include <deque>
#include <memory>
#include <vector>
//#  include <QSqlDatabase>
//#  include <QSqlQuery>
#include <QString>
#include <QVector>
#include <set>
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/notification/process_manager.hh"
#include "com/centreon/broker/notification/state.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class stream stream.hh "com/centreon/broker/notification/stream.hh"
 *  @brief Notification stream.
 *
 *  Stream events into the notification database.
 */
class stream : public io::stream {
 public:
  stream(std::string const& type,
         std::string const& host,
         unsigned short port,
         std::string const& user,
         std::string const& password,
         std::string const& centreon_db,
         bool check_replication,
         node_cache& cache);
  stream(stream const& s);
  ~stream();
  static void initialize();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void update();
  int write(std::shared_ptr<io::data> const& d);

 private:
  stream& operator=(stream const& s);
  void _open_db(std::unique_ptr<mysql>& db,
                std::string const& t,
                std::string const& host,
                unsigned short port,
                std::string const& user,
                std::string const& password,
                std::string const& db_name,
                bool check_replication);
  //    void         _clone_db(
  //                   std::unique_ptr<mysql>& db,
  //                   std::unique_ptr<mysql> const& db_to_clone,
  //                   QString const& id);
  void _update_objects_from_db();
  void _process_service_status_event(neb::service_status const& event);
  void _process_host_status_event(neb::host_status const& event);
  void _process_issue_parent_event(correlation::issue_parent const& event);
  void _process_ack(neb::acknowledgement const& event);
  void _process_downtime(neb::downtime const& event);

  std::unique_ptr<mysql> _centreon_db;
  std::unique_ptr<notification_scheduler> _notif_scheduler;

  state _state;
  node_cache& _node_cache;
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_STREAM_HH
