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

#ifndef CCB_INFLUXDB_INFLUXDB_HH
#  define CCB_INFLUXDB_INFLUXDB_HH

#  include <string>
#  include <memory>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/storage/status.hh"

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
     *  Write an event to the queue of event pending.
     *  @param[in] m  The event.
     */
    virtual void  write(storage::status const& m) = 0;

    /**
     *  Commit all the events pending to the db.
     */
    virtual void  commit() = 0;
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_INFLUXDB_HH
