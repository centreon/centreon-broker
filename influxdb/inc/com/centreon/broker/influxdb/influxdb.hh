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

#ifndef CCB_INFLUXDB_INFLUXDB_HH
#  define CCB_INFLUXDB_INFLUXDB_HH

#  include <string>
#  include <memory>
#  include <QTcpSocket>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"

CCB_BEGIN()

namespace         influxdb {
  /**
   *  @class influxdb influxdb.hh "com/centreon/broker/influxdb/influxdb.hh"
   *  @brief Interface for influxdb connection manager.
   *
   *  This object is an interface all influxdb connection manager needs
   *  to implements.
   */
  class           influxdb {
  public:
    virtual       ~influxdb() {}

    /**
     *  Clear all the events pending.
     */
    virtual void  clear() = 0;

    /**
     *  Write an event to the queue of event pending.
     *  @param[in] m  The event.
     */
    virtual void  write(storage::metric const& m) = 0;

    /**
     *  Commit all the events pending to the db.
     */
    virtual void  commit() = 0;
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_INFLUXDB_HH
