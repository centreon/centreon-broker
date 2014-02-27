/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_SERVICE_LISTENER_HH
#  define CCB_BAM_SERVICE_LISTENER_HH

#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace             neb {
  class               service_status;
}

namespace             bam {
  /**
   *  @class service_listener service_listener.hh "com/centreon/broker/bam/service_listener.hh"
   *  @brief Listen to service state change.
   *
   *  This interface is used by classes wishing to listen to service
   *  state change.
   */
  class               service_listener {
  public:
                      service_listener();
                      service_listener(service_listener const& right);
    virtual           ~service_listener();
    service_listener& operator=(service_listener const& right);

    /**
     *  Notify of service update.
     *
     *  @param[in] status Service status.
     */
    virtual void      service_update(
                        misc::shared_ptr<neb::service_status> const& status) = 0;
  };
}

CCB_END()

#endif // !CCB_BAM_SERVICE_LISTENER_HH
