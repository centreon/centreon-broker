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

#ifndef CCB_NOTIFICATION_LOADERS_DOWNTIME_LOADER_HH
#  define CCB_NOTIFICATION_LOADERS_DOWNTIME_LOADER_HH

#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/notification/builders/downtime_builder.hh"

CCB_BEGIN()

namespace  notification {
  /**
   *  @class downtime_loader downtime_loader.hh "com/centreon/broker/notification/loaders/downtime_loader.hh"
   *  @brief Loader for downtime objects.
   *
   *  This loader loads the downtimes from the database.
   */
  class    downtime_loader {
  public:
           downtime_loader();

    void   load(QSqlDatabase* db, downtime_builder* output);
    void   new_downtime(
             neb::downtime& new_downtime,
             downtime_builder& output);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_LOADERS_DOWNTIME_LOADER_HH
