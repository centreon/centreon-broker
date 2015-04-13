/*
** Copyright 2009-2013 Merethis
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

#ifndef CCB_CORRELATION_PARSER_HH
#  define CCB_CORRELATION_PARSER_HH

#  include <QMap>
#  include <QPair>
#  include <QString>
#  include <QXmlDefaultHandler>
#  include "com/centreon/broker/correlation/node.hh"
#  include "com/centreon/broker/namespace.hh"

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
  public:
            parser();
            parser(parser const& p);
            ~parser();
    parser& operator=(parser const& p);
    void    parse(
              QString const& filename,
              QMap<QPair<unsigned int, unsigned int>, node>& nodes,
              bool recursive = false);

  private:
    void    _auto_services_dependencies();
    node*   _find_node(
              char const* host_id,
              char const* service_id);
    bool    characters(QString const& ch);
    bool    endElement(
              QString const& uri,
              QString const& localname,
              QString const& qname);
    bool    startElement(
              QString const& uri,
              QString const& localname,
              QString const& qname,
              QXmlAttributes const& attrs);

    bool    _in_include;
    bool    _in_root;
    QString _include_file;
    QMap<QPair<unsigned int, unsigned int>, node>*
            _nodes;
  };
}

CCB_END()

#endif // !CORRELATION_PARSER_HH
