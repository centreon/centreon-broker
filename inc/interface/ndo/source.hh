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

#ifndef INTERFACE_NDO_SOURCE_HH_
# define INTERFACE_NDO_SOURCE_HH_

# include <string>
# include "interface/ndo/base.hh"
# include "interface/source.hh"

namespace                    interface {
  namespace                  ndo {
    /**
     *  @class source source.hh "interface/ndo/source.hh"
     *  @brief NDO input source.
     *
     *  The interface::ndo::source class converts an input stream into
     *  events using a modified version of the NDO protocol.
     */
    class                    source : virtual public base,
                                      virtual public interface::source {
     private:
                             source(source const& s);
      source&                operator=(source const& s);

     public:
                             source(io::stream* s);
      virtual                ~source();
      virtual void           close();
      virtual events::event* event();
    };
  }
}

#endif /* !INTERFACE_NDO_SOURCE_HH_ */
