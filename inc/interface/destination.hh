/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef INTERFACE_DESTINATION_HH_
# define INTERFACE_DESTINATION_HH_

// Forward declaration.
namespace          events
{ class            event; }

namespace          interface {
  /**
   *  @class destination destination.hh "interface/destination.hh"
   *  @brief Base interface for event-storing objects.
   *
   *  interface::destination is the base interface used to store objects
   *  in an output destination. The underlying destination can either be
   *  a XML stream, a database, ...
   *
   *  @see db::db
   *  @see file::file
   *  @see ndo::ndo
   *  @see xml::xml
   */
  class          destination {
   protected:
                 destination();
                 destination(destination const& dest);
    destination& operator=(destination const& dest);

   public:
    virtual      ~destination();
    virtual void close() = 0;
    virtual void event(events::event* e) = 0;
  };
}

#endif /* !INTERFACE_DESTINATION_HH_ */
