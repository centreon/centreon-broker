/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_INSTANCE_BROADCAST_HH
#  define CCB_INSTANCE_BROADCAST_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/io/events.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class instance_broadcast instance_broadcast.hh "com/centreon/broker/instance_broadcast.hh"
 *  @brief Broadcast the instance information at startup.
 */
class                  instance_broadcast : public io::data {
public:
                       instance_broadcast();
                       instance_broadcast(instance_broadcast const& other);
                       ~instance_broadcast();
  instance_broadcast&  operator=(instance_broadcast const& other);
  unsigned int         type() const;

  /**
   *  Get the event type.
   *
   *  @return The event type.
   */
  static unsigned int static_type() {
    return (io::events::data_type<
                          io::events::internal,
                          io::events::de_instance_broadcast>::value);
  }

  bool                enabled;
  QString             instance_name;

  static mapping::entry const
                      entries[];
  static io::event_info::event_operations const
                      operations;

  static void         load();

private:
  void                _internal_copy(instance_broadcast const& right);
};

CCB_END()

#endif // !CCB_INSTANCE_BROADCAST_HH
