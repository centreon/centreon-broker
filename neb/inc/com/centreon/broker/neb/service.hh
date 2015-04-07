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

#ifndef CCB_NEB_SERVICE_HH
#  define CCB_NEB_SERVICE_HH

#  include <ctime>
#  include <QString>
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/host_service.hh"
#  include "com/centreon/broker/neb/service_status.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class service service.hh "com/centreon/broker/neb/service.hh"
   *  @brief Service as handled by the scheduling engine.
   *
   *  Holds full data regarding a service.
   *
   *  @see host_service
   *  @see service_status
   */
  class            service : public host_service,
                             public service_status {
  public:
                   service();
                   service(service_status const& ss);
                   service(service const& other);
                   ~service();
    service&       operator=(service const& other);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    bool           flap_detection_on_critical;
    bool           flap_detection_on_ok;
    bool           flap_detection_on_unknown;
    bool           flap_detection_on_warning;
    bool           is_volatile;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(service const& other);
    void           _zero_initialize();
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_HH
