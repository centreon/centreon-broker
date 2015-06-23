/*
** Copyright 2009-2013,2015 Merethis
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

#ifndef CCB_NEB_HOST_HH
#  define CCB_NEB_HOST_HH

#  include <QString>
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/host_service.hh"
#  include "com/centreon/broker/neb/host_status.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class host host.hh "com/centreon/broker/neb/host.hh"
   *  @brief Host within the scheduling engine.
   *
   *  The scheduling engine has two main objects that can be
   *  manipulated : host and service. An host object holds every
   *  parameter related to a host machine.
   */
  class            host : public host_service, public host_status {
  public:
                   host();
                   host(host_status const& hs);
                   host(host const& other);
                   ~host();
    host&          operator=(host const& other);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    QString        address;
    QString        alias;
    bool           flap_detection_on_down;
    bool           flap_detection_on_unreachable;
    bool           flap_detection_on_up;
    QString        host_name;
    unsigned int   poller_id;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(host const& other);
    void           _zero_initialize();
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_HH
