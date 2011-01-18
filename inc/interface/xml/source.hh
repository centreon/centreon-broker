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

#ifndef EVENTS_XML_SOURCE_HH_
# define EVENTS_XML_SOURCE_HH_

# include <memory>
# include "interface/source.hh"
# include "io/stream.hh"

namespace                       interface {
  namespace                     xml {
    /**
     *  @class source source.hh "interface/xml/source.hh"
     *  @brief XML input source.
     *
     *  The interface::xml::source class converts an input stream into
     *  events using the XML protocol.
     */
    class                       source : public interface::source {
     private:
      std::auto_ptr<io::stream> _stream;
                                source(source const& s);
      source&                   operator=(source const& s);

     public:
                                source(io::stream* s);
      virtual                   ~source();
      virtual void              close();
      virtual events::event*    event();
    };
  }
}

#endif /* !EVENTS_XML_SOURCE_HH_ */
