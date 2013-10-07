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

#ifndef CCB_CORRELATION_HOST_STATE_HH
#  define CCB_CORRELATION_HOST_STATE_HH

#  include "com/centreon/broker/correlation/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class host_state host_state.hh "com/centreon/broker/correlation/host_state.hh"
   *  @brief Host state.
   *
   *  State of an host at a given time.
   */
  class            host_state : public state {
  public:
                   host_state();
                   host_state(host_state const& hs);
                   ~host_state();
    host_state&    operator=(host_state const& hs);
    bool           operator==(host_state const& hs) const;
    bool           operator!=(host_state const& hs) const;
    unsigned int   type() const;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_HOST_STATE_HH
