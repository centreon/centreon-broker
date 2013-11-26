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

#ifndef CCB_IO_PROPERTIES_HH
#  define CCB_IO_PROPERTIES_HH

#  include <list>
#  include <map>
#  include <string>
#  include "com/centreon/broker/io/property.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              io {
  /**
   *  @class properties properties.hh "com/centreon/broker/io/properties.hh"
   *  @brief Properties of endpoints.
   *
   *  This class is used to gather information about endpoints.
   */
  class                properties {
  public:
    typedef std::map<std::string, property>::iterator iterator;
    typedef std::map<std::string, property>::const_iterator const_iterator;

                       properties(std::string const& name = "");
                       properties(properties const& right);
                       ~properties() throw ();
    properties&        operator=(properties const& right);
    property const&    operator[](std::string const& name) const;
    property&          operator[](std::string const& name);
    iterator           begin();
    const_iterator     begin() const;
    std::list<properties> const&
                       children() const;
    std::list<properties>&
                       children();
    iterator           end();
    const_iterator     end() const;
    property const&    get(std::string const& name) const;
    std::string const& name() const throw ();
    void               name(std::string const& name);

  private:
    std::list<properties>
                       _children;
    std::string        _name;
    std::map<std::string, property>
                       _properties;
  };
}

CCB_END()

#endif // !CCB_IO_PROPERTIES_HH
