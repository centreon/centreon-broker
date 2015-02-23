/*
** Copyright 2012-2013 Merethis
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

#ifndef CCB_STORAGE_REBUILD_HH
#  define CCB_STORAGE_REBUILD_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class rebuild rebuild.hh "com/centreon/broker/storage/rebuild.hh"
   *  @brief Rebuild event.
   *
   *  This event is generated when some graph need to be rebuild.
   */
  class            rebuild : public io::data {
  public:
                   rebuild();
                   rebuild(rebuild const& right);
                   ~rebuild();
    rebuild&       operator=(rebuild const& right);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    bool           end;
    unsigned int   id;
    bool           is_index;


    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(rebuild const& right);
  };
}

CCB_END()

#endif // !CCB_STORAGE_REBUILD_HH
