/*
** Copyright 2009-2012,2015 Merethis
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

#ifndef CCB_NEB_HOST_DEPENDENCY_HH
#  define CCB_NEB_HOST_DEPENDENCY_HH

#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/neb/dependency.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            neb {
  /**
   *  @class host_dependency host_dependency.hh "com/centreon/broker/neb/host_dependency.hh"
   *  @brief Host dependency relationship.
   *
   *  Defines a dependency between two hosts.
   */
  class              host_dependency : public dependency {
  public:
                     host_dependency();
                     host_dependency(host_dependency const& other);
                     ~host_dependency();
    host_dependency& operator=(host_dependency const& other);
    unsigned int     type() const;

    static mapping::entry const
                     entries[];
    static io::event_info::event_operations const
                     operations;
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_DEPENDENCY_HH
