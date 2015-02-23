/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NOTIFICATION_LOADERS_NOTIFICATION_METHOD_LOADER_HH
#  define CCB_NOTIFICATION_LOADERS_NOTIFICATION_METHOD_LOADER_HH

#  include <string>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/notification_method_builder.hh"

CCB_BEGIN()

namespace       notification {
  /**
   *  @class notification_method_loader notification_method_loader.hh "com/centreon/broker/notification/loaders/notification_method_loader.hh"
   *  @brief Loader for notification method objects.
   *
   *  This loader loads the notifcation methods from the database.
   */
  class         notification_method_loader {
  public:
                notification_method_loader();

    void        load(QSqlDatabase* db, notification_method_builder* output);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_LOADERS_NOTIFICATION_METHOD_LOADER_HH
