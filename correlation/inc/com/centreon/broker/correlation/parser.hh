/*
** Copyright 2009-2011 Merethis
**
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

# include <QMap>
# include <QPair>
# include <QString>
# include <QXmlDefaultHandler>
# include "com/centreon/broker/correlation/node.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   correlation {
  /**
   *  @class parser parser.hh "com/centreon/broker/correlation/parser.hh"
   *  @brief Parse the correlation file.
   *
   *  Parse the correlation file which contains all host and services
   *  relationships.
   */
  class     parser : private QXmlDefaultHandler {
   private:
    bool    _in_root;
    bool    _is_retention;
    QMap<QPair<unsigned int, unsigned int>, node>*
            _nodes;
    void    _auto_services_dependencies();
    node*   _find_node(
              char const* host_id,
              char const* service_id);
    bool    startElement(
              QString const& uri,
              QString const& localname,
              QString const& qname,
              QXmlAttributes const& attrs);

   public:
            parser();
            parser(parser const& p);
            ~parser();
    parser& operator=(parser const& p);
    void    parse(
              QString const& filename,
              bool is_retention,
              QMap<QPair<unsigned int, unsigned int>, node>& nodes);
  };
}

CCB_END()

#endif /* !CORRELATION_PARSER_HH_ */
