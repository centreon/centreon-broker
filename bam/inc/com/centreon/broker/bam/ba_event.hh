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

#ifndef CCB_BAM_BA_EVENT_HH
#  define CCB_BAM_BA_EVENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class ba_event ba_event.hh "com/centreon/broker/bam/ba_event.hh"
   *  @brief Ba event
   *
   */
  class          ba_event : public io::data {
  public:
                 ba_event();
                 ba_event(ba_event const& other);
                 ~ba_event();
    ba_event&    operator=(ba_event const& other);
    bool         operator==(ba_event const& other) const;
    unsigned int type() const;
    static unsigned int
                 static_type();

    unsigned int ba_id;
    double       first_level;
    timestamp    end_time;
    bool         in_downtime;
    timestamp    start_time;
    short        status;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void         _internal_copy(ba_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_BA_EVENT_HH
