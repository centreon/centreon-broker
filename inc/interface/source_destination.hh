/*
** Copyright 2009-2010 MERETHIS
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

#ifndef INTERFACE_SOURCE_DESTINATION_HH_
# define INTERFACE_SOURCE_DESTINATION_HH_

# include "interface/destination.hh"
# include "interface/source.hh"

namespace               interface {
  /**
   *  @class source_destination source_destination.h "interface/source_destination.h"
   *  @brief Base interface for objects that can generate and store
   *         events.
   *
   *  source_destination is at the same time a source and a destination.
   *  It regroups the functionnality of both classes within one single
   *  interface.
   *
   *  @see source
   *  @see destination
   */
  class                 source_destination : virtual public interface::source,
                                             virtual public interface::destination {
   protected:
                        source_destination();
                        source_destination(source_destination const& sd);
    source_destination& operator=(source_destination const& sd);

   public:
    virtual             ~source_destination();
    virtual void        close() = 0;
  };
}

#endif /* !INTERFACE_SOURCE_DESTINATION_HH_ */
