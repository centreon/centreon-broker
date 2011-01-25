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
# include <QtXml>
# include <queue>
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
    class                        source : public interface::source,
                                          public QXmlDefaultHandler,
                                          public QXmlInputSource {
     private:
      std::queue<events::event*> _events;
      QXmlSimpleReader           _parser;
      std::auto_ptr<io::stream>  _stream;
                                 source(source const& s);
      source&                    operator=(source const& s);
      void                       fetchData();
      template                   <typename T>
      void                       handle_event(QXmlAttributes const& attrs);
      bool                       startElement(QString const& nspace,
                                   QString const& localname,
                                   QString const& qname,
                                   QXmlAttributes const& attrs);

     public:
                                 source(io::stream* s);
      virtual                    ~source();
      virtual void               close();
      virtual events::event*     event();
    };
  }
}

#endif /* !EVENTS_XML_SOURCE_HH_ */
