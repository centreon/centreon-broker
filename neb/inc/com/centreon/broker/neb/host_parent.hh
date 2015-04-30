/*
** Copyright 2009-2013,2015 Merethis
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

#ifndef CCB_NEB_HOST_PARENT_HH
#  define CCB_NEB_HOST_PARENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class host_parent host_parent.hh "com/centreon/broker/neb/host_parent.hh"
   *  @brief Define a parent of a host.
   *
   *  Define a certain host to be the parent of another host.
   */
  class            host_parent : public io::data {
  public:
                   host_parent();
                   host_parent(host_parent const& other);
                   ~host_parent();
    host_parent&   operator=(host_parent const& other);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    bool           enabled;
    unsigned int   host_id;
    unsigned int   parent_id;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_PARENT_HH
