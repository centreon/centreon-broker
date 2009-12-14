/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef CONFIGURATION_INTERFACE_H_
# define CONFIGURATION_INTERFACE_H_

# include <string>

namespace       Configuration
{
  /**
   *  \class Interface interface.h "configuration/interface.h"
   *  \brief Hold configuration of an interface.
   *
   *  An interface is an external source or destination for events. This can
   *  either be an XML stream, a database, a file, ... This class holds the
   *  configuration of an interface.
   */
  class         Interface
  {
   private:
    void        InternalCopy(const Interface& interface);

   public:
    enum        Protocol
    {
      UNKNOWN_PROTO = 0,
      NDO,
      XML
    };
    enum        Type
    {
      UNKNOWN_TYPE = 0,
      FILE,
      IPV4,
      IPV6,
      MYSQL,
      ORACLE,
      POSTGRESQL,
      UNIX
    };
    std::string db;
    std::string filename;
    std::string host;
    std::string interface;
    std::string name;
    std::string password;
    short       port;
    Protocol    protocol;
    std::string socket;
    Type        type;
    std::string user;
                Interface();
                Interface(const Interface& interface);
                ~Interface();
    Interface&  operator=(const Interface& interface);
    bool        operator==(const Interface& interface) const;
    bool        operator!=(const Interface& interface) const;
    bool        operator<(const Interface& interface) const;
  };
}

#endif /* !CONFIGURATION_INTERFACE_H_ */
