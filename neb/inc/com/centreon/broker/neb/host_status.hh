/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_HOST_STATUS_HH
#  define CCB_NEB_HOST_STATUS_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/host_service_status.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class host_status host_status.hh "com/centreon/broker/neb/host_status.hh"
   *  @brief host_status represents a status change of an host.
   *
   *  host_status are generated when the status of an host change.
   *  Appropriate fields should be updated.
   */
  class            host_status : public host_service_status {
  public:
                   host_status();
                   host_status(host_status const& hs);
    virtual        ~host_status();
    host_status&   operator=(host_status const& hs);
    unsigned int   type() const;

    timestamp      last_time_down;
    timestamp      last_time_unreachable;
    timestamp      last_time_up;

  private:
    void           _internal_copy(host_status const& hs);
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_STATUS_HH
