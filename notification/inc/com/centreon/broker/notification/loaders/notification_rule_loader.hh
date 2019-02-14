/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_NOTIFICATION_LOADERS_NOTIFICATION_RULE_LOADER_HH
#  define CCB_NOTIFICATION_LOADERS_NOTIFICATION_RULE_LOADER_HH

#  include <string>
#  include "com/centreon/broker/mysql.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/notification_rule_builder.hh"

CCB_BEGIN()

namespace       notification {
  /**
   *  @class notification_rule_loader notification_rule_loader.hh "com/centreon/broker/notification/loaders/notification_rule_loader.hh"
   *  @brief Loader for notification rules.
   *
   *  This loader loads the notifcation rules from the database.
   */
  class         notification_rule_loader {
  public:
                notification_rule_loader();

    void        load(mysql* db, notification_rule_builder* output);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_LOADERS_NOTIFICATION_RULE_LOADER_HH
