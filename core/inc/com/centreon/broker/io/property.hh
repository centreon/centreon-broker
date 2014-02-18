/*
** Copyright 2013 Merethis
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

#ifndef CCB_IO_PROPERTY_HH
#  define CCB_IO_PROPERTY_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              io {
  /**
   *  @class property property.hh "com/centreon/broker/io/property.hh"
   *  @brief Single property.
   *
   *  Value and attributes of a single property.
   */
  class                property {
  public:
                       property(
                         std::string const& output = "",
                         std::string const& perfdata = "",
                         bool graphable = false);
                       property(property const& right);
                       ~property() throw ();
    property&          operator=(property const& right);
    std::string const& get_output() const throw ();
    std::string const& get_perfdata() const throw ();
    bool               is_graphable() const throw ();
    void               set_graphable(bool graphable = true);
    void               set_output(std::string const& output);
    void               set_perfdata(std::string const& perfdata);

  private:
    bool               _graphable;
    std::string        _output;
    std::string        _perfdata;
  };
}

CCB_END()

#endif // !CCB_IO_PROPERTY_HH
