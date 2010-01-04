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

#ifndef INTERFACE_FACTORY_H_
# define INTERFACE_FACTORY_H_

// Forward declaration.
namespace                   Configuration
{ class                     Interface; }
namespace                   IO
{ class                     Acceptor;
  class                     Stream; }

namespace                   Interface
{
  // Forward declarations.
  class                     Destination;
  class                     Source;
  class                     SourceDestination;

  /**
   *  \class Factory factory.h "interface/factory.h"
   *  \brief Build an interface from its configuration.
   *
   *  The interface factory takes a configuration in input and produce the
   *  corresponding interface object.
   *
   *  \see Source
   *  \see Destination
   *  \see SourceDestination
   */
  class                     Factory
  {
   private:
                            Factory();
                            Factory(const Factory& factory);
                            ~Factory();
    Factory&                operator=(const Factory& factory);
    IO::Stream*             IPv4Connector(const Configuration::Interface& i);
    IO::Stream*             IPv6Connector(const Configuration::Interface& i);
    IO::Stream*             TLSConnector(const Configuration::Interface& i,
                                         IO::Stream* stream);
    IO::Stream*             UnixConnector(const Configuration::Interface& i);

   public:
    IO::Acceptor*           Acceptor(const Configuration::Interface& i);
    Interface::Destination* Destination(const Configuration::Interface& i);
    static Factory&         Instance();
    Interface::Source*      Source(const Configuration::Interface& i);
    Interface::SourceDestination*
                            SourceDestination(
                              const Configuration::Interface& i);
  };
}

#endif /* !INTERFACE_FACTORY_H_ */
