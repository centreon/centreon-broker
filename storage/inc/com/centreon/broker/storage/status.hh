/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_STORAGE_STATUS_HH
#  define CCB_STORAGE_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class status status.hh "com/centreon/broker/storage/status.hh"
   *  @brief Status data used to generate status graphs.
   *
   *  Status data event, mainly used to generate status graphs.
   */
  class            status : public io::data {
  public:
                   status();
                   status(status const& s);
                   ~status();
    status&        operator=(status const& s);
    unsigned int   type() const;

    timestamp      ctime;
    unsigned int   index_id;
    unsigned int   interval;
    bool           is_for_rebuild;
    timestamp      rrd_len;
    short          state;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(status const& s);
  };
}

CCB_END()

#endif // !CCB_STORAGE_STATUS_HH
