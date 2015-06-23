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

#ifndef CCB_CORRELATION_STATE_HH
#  define CCB_CORRELATION_STATE_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"

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
  public:
                 state();
                 state(state const& s);
    virtual      ~state();
    state&       operator=(state const& s);
    bool         operator==(state const& s) const;
    bool         operator!=(state const& s) const;

    unsigned int type() const;
    static unsigned int
                 static_type();

    timestamp    ack_time;
    int          current_state;
    timestamp    end_time;
    unsigned int host_id;
    bool         in_downtime;
    unsigned int poller_id;
    unsigned int service_id;
    timestamp    start_time;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void         _internal_copy(state const& s);
  };
}

CCB_END()

#endif // !CCB_CORRELATION_STATE_HH
