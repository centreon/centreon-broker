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

#ifndef INTERFACE_SOURCE_HH_
# define INTERFACE_SOURCE_HH_

// Forward declaration.
namespace                    events
{ class                      event; }

namespace                    interface {
  /**
   *  @class source source.hh "interface/source.hh"
   *  @brief Base interface for event-generating objects.
   *
   *  interface::source is the base interface used to get objects from
   *  an input source. The underlying source can be either a XML stream,
   *  a database, ...
   *
   *  @see db::db
   *  @see file::file
   *  @see ndo::ndo
   *  @see xml::xml
   */
  class                    source {
   protected:
                           source();
                           source(source const& s);
    source&                operator=(source const& s);

   public:
    virtual                ~source();
    virtual void           close() = 0;
    virtual events::event* event() = 0;
  };
}

#endif /* !INTERFACE_SOURCE_HH_ */
