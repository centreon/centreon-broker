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

#ifndef CCB_NOTIFICATION_LOADERS_CONTACT_LOADER_HH
#  define CCB_NOTIFICATION_LOADERS_CONTACT_LOADER_HH

#  include <string>
#  include "com/centreon/broker/notification/builders/contact_builder.hh"
#  include "com/centreon/broker/mysql.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace  notification {
  /**
   *  @class contact_loader contact_loader.hh "com/centreon/broker/notification/loaders/contact_loader.hh"
   *  @brief Loader for contact objects.
   *
   *  This loader loads the contacts from the database.
   */
  class    contact_loader {
  public:
           contact_loader();

    void   load(mysql* db, contact_builder* output);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_LOADERS_CONTACT_LOADER_HH
