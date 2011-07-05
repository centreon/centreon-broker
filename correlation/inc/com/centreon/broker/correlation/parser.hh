/*
** Copyright 2009-2011 Merethis
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
*/

#ifndef CCB_CORRELATION_PARSER_HH_
# define CCB_CORRELATION_PARSER_HH_

# include <map>
# include <QString>
# include <QtXml>
# include "com/centreon/broker/correlation/node.hh"

namespace                      com {
  namespace                    centreon {
    namespace                  broker {
      namespace                correlation {
        /**
         *  @class parser parser.hh "com/centreon/broker/correlation/parser.hh"
         *  @brief Parse the correlation file.
         *
         *  Parse the correlation file which contains all host and services
         *  relationships.
         */
        class                  parser : private QXmlDefaultHandler {
         private:
          std::map<unsigned int, node>* _hosts;
          bool                 _in_root;
          std::map<std::pair<unsigned int, unsigned int>, node>*
                               _services;
          void                 _auto_services_dependencies();
          node*                _find_node(char const* host_id,
                                 char const* service_id);
          bool                 startElement(QString const& uri,
                                 QString const& localname,
                                 QString const& qname,
                                 QXmlAttributes const& attrs);

         public:
                               parser();
                               parser(parser const& p);
                               ~parser();
          parser&              operator=(parser const& p);
          void                 parse(QString const& filename,
                                 std::map<unsigned int, node>& hosts,
                                 std::map<std::pair<unsigned int, unsigned int>, node>& services);
        };
      }
    }
  }
}

#endif /* !CORRELATION_PARSER_HH_ */
