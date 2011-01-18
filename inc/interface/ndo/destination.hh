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

#ifndef INTERFACE_NDO_DESTINATION_HH_
# define INTERFACE_NDO_DESTINATION_HH_

# include <memory>
# include "interface/destination.hh"
# include "interface/ndo/base.hh"

namespace          interface {
  namespace        ndo {
    /**
     *  @class destination destination.hh "interface/ndo/destination.hh"
     *  @brief NDO destination output.
     *
     *  The interface::ndo::destination class converts events to an
     *  output stream using a modified version of the NDO protocol.
     */
    class          destination : virtual public base,
                                 virtual public interface::destination {
     private:
                   destination(destination const& dest);
      destination& operator=(destination const& dest);

     public:
                   destination(io::stream* s);
      virtual      ~destination();
      virtual void close();
      virtual void event(events::event* e);
    };
  }
}

#endif /* !INTERFACE_NDO_DESTINATION_HH_ */
