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

#ifndef CCB_NEB_HOST_CHECK_HH
#  define CCB_NEB_HOST_CHECK_HH

#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/neb/check.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class host_check host_check.hh "com/centreon/broker/neb/host_check.hh"
   *  @brief Check that has been executed on a host.
   *
   *  Once a check has been executed on a host, an object of this class
   *  is sent.
   */
  class            host_check : public check {
  public:
                   host_check();
                   host_check(host_check const& other);
    virtual        ~host_check();
    host_check&    operator=(host_check const& other);
    unsigned int   type() const;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_CHECK_HH
