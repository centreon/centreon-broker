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

#ifndef INTERFACE_XML_DESTINATION_HH_
# define INTERFACE_XML_DESTINATION_HH_

# include <memory>
# include "interface/destination.hh"

// Forward declaration.
namespace          io {
  class            stream;
}

namespace          interface {
  namespace        xml {
    /**
     *  @class destination destination.hh "interface/xml/destination.hh"
     *  @brief XML destination.
     *
     *  The interface::xml::destination class converts events in their
     *  generic representation into an XML stream.
     */
    class          destination : public interface::destination {
     private:
      std::auto_ptr<io::stream>
                   _stream;
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

#endif /* !INTERFACE_XML_DESTINATION_HH_ */
