/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_PROCESS_MANAGER_HH
#define CCB_NOTIFICATION_PROCESS_MANAGER_HH

#include <QThread>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include "com/centreon/broker/notification/process.hh"

namespace com {
namespace centreon {
namespace broker {
namespace notification {
/**
 *  @class process_manager process_manager.hh
 * "com/centreon/broker/notification/processprocess_manager.hh"
 *  @brief Manage the processes.
 */
class process_manager : public QObject {
  Q_OBJECT

 public:
  static process_manager& instance();
  static void release();

  void create_process(std::string const& command, unsigned int timeout = 0);

  QThread& get_thread();

 public slots:
  void process_finished(process&);
  void process_timeouted(process&);

 private:
  static process_manager* _instance_ptr;
  process_manager();
  process_manager(process_manager const&);
  process_manager& operator=(process_manager const&);

  std::recursive_mutex _process_list_mutex;
  std::set<process*> _process_list;

  std::unique_ptr<QThread> _thread;
};
}  // namespace notification
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_NOTIFICATION_PROCESS_MANAGER_HH
