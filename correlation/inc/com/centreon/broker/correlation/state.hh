/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_CORRELATION_STATE_HH_
# define CCB_CORRELATION_STATE_HH_

# include <time.h>
# include "com/centreon/broker/io/data.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        correlation {
  /**
   *  @class state state.hh "com/centreon/broker/correlation/state.hh"
   *  @brief State of a checkpoint.
   *
   *  This class represent the state of a specific checkpoint for
   *  a given time.
   */
  class          state : public io::data {
   private:
    void         _internal_copy(state const& s);

   public:
    int          current_state;
    time_t       end_time;
    unsigned int host_id;
    bool         in_downtime;
    unsigned int service_id;
    time_t       start_time;
                 state();
                 state(state const& s);
    virtual      ~state();
    state&       operator=(state const& s);
  };
}

CCB_END()

#endif /* !CCB_CORRELATION_STATE_HH_ */
