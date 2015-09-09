/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
    static void _sanity_circular_check(
              QMap<QPair<unsigned int, unsigned int>, node> const& nodes);

    bool    _in_include;
    bool    _in_root;
    QString _include_file;
    QMap<QPair<unsigned int, unsigned int>, node>*
            _nodes;
  };
}

CCB_END()

#endif // !CORRELATION_PARSER_HH
