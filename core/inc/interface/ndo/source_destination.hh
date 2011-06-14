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

#ifndef INTERFACE_NDO_SOURCE_DESTINATION_HH_
# define INTERFACE_NDO_SOURCE_DESTINATION_HH_

# include "interface/ndo/destination.hh"
# include "interface/ndo/source.hh"
# include "interface/source_destination.hh"

namespace                 interface {
  namespace               ndo {
    /**
     *  @class source_destination source_destination.hh "interface/ndo/source_destination.hh"
     *  @brief Act as source and destination of/for events.
     *
     *  Sum capabilities of interface::ndo::source and
     *  interface::ndo::destination classes.
     *
     *  @see interface::ndo::source
     *  @see interface::ndo::destination
     */
    class                 source_destination : public interface::source_destination,
                                               public interface::ndo::source,
                                               public interface::ndo::destination {
     private:
                          source_destination(
                            source_destination const& sd);
      source_destination& operator=(source_destination const& sd);

     public:
                          source_destination(io::stream* s);
                          ~source_destination();
      void                close();
    };
  }
}

#endif /* !INTERFACE_NDO_SOURCE_DESTINATION_HH_ */
