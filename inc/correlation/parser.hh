/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef CORRELATION_PARSER_HH_
# define CORRELATION_PARSER_HH_

# include <map>
# include <xercesc/sax2/DefaultHandler.hpp>
# include "correlation/node.h"
# include "interface/xml/tinyxml.h"

namespace                Correlation
{
  /**
   *  @class Parser parser.hh "correlation/parser.hh"
   *  @brief Parse the correlation file.
   *
   *  Parse the correlation file which contains all host and services
   *  relationships.
   */
  class                  parser : private xercesc::DefaultHandler
  {
   private:
    std::map<int, Node>* _hosts;
    bool                 _in_root;
    std::map<std::pair<int, int>, Node>*
                         _services;
    Node*                _find_node(char const* host_id, char const* service_id);
    void                 startElement(XMLCh const* const uri,
                                      XMLCh const* const localname,
                                      XMLCh const* const qname,
                                      xercesc::Attributes const& attrs);

   public:
                         parser();
                         parser(parser const& p);
                         ~parser();
    parser&              operator=(parser const& p);
    void                 parse(const char* filename,
                           std::map<int, Node>& hosts,
                           std::map<std::pair<int, int>, Node>& services);
  };
}

#endif /* !CORRELATION_PARSER_HH_ */
