/*
** Copyright 2009-2013 Merethis
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
                   host(host const& h);
                   ~host();
    host&          operator=(host const& h);
    unsigned int   type() const;

    QString        address;
    QString        alias;
    bool           flap_detection_on_down;
    bool           flap_detection_on_unreachable;
    bool           flap_detection_on_up;
    QString        host_name;
    unsigned int   instance_id;
    bool           notify_on_down;
    bool           notify_on_unreachable;
    bool           stalk_on_down;
    bool           stalk_on_unreachable;
    bool           stalk_on_up;
    QString        statusmap_image;

  private:
    void           _internal_copy(host const& h);
    void           _zero_initialize();
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_HH
