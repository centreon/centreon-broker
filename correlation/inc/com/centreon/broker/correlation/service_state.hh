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

#ifndef CCB_CORRELATION_SERVICE_STATE_HH
#  define CCB_CORRELATION_SERVICE_STATE_HH

#  include "com/centreon/broker/correlation/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class service_state service_state.hh "com/centreon/broker/correlation/service_state.hh"
   *  @brief Service state.
   *
   *  State of a service at a given time.
   */
  class            service_state : public state {
   public:
                   service_state();
                   service_state(service_state const& ss);
                   ~service_state();
    service_state& operator=(service_state const& ss);
    bool           operator==(service_state const& ss) const;
    bool           operator!=(service_state const& ss) const;
    QString const& type() const;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_SERVICE_STATE_HH
