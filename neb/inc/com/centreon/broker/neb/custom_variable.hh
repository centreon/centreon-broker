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

#ifndef CCB_NEB_CUSTOM_VARIABLE_HH
#  define CCB_NEB_CUSTOM_VARIABLE_HH

#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/neb/custom_variable_status.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            neb {
  /**
   *  @class custom_variable custom_variable.hh "com/centreon/broker/neb/custom_variable.hh"
   *  @brief Custom variable definition.
   *
   *  Nagios allows users to declare variables attached to a
   *  specific host or service.
   */
  class              custom_variable : public custom_variable_status {
  public:
                     custom_variable();
                     custom_variable(custom_variable const& other);
                     ~custom_variable();
    custom_variable& operator=(custom_variable const& other);
    unsigned int     type() const;
    static unsigned int
                     static_type();

    bool             enabled;
    short            var_type;

    static mapping::entry const
                     entries[];
    static io::event_info::event_operations const
                     operations;

  private:
    void             _internal_copy(custom_variable const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_CUSTOM_VARIABLE_HH
