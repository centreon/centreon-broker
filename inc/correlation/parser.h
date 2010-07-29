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

#ifndef CORRELATION_PARSER_H_
# define CORRELATION_PARSER_H_

# include <map>
# include "correlation/node.h"
# include "interface/xml/tinyxml.h"

namespace   Correlation
{
  /**
   *  \class Parser parser.h "correlation/parser.h"
   *  \brief Parse the correlation file.
   *
   *  Parse the correlation file which contains all host and services
   *  relationships.
   */
  class     Parser : public TiXmlVisitor
  {
   private:
    std::map<int, Node>* hosts_;
    std::map<std::pair<int, int>, Node>*
                         services_;
    Node*                FindNode(const char* host_id, const char* service_id);
    bool                 VisitEnter(const TiXmlElement& elem,
				    const TiXmlAttribute* attr);

   public:
            Parser();
            Parser(const Parser& parser);
            ~Parser();
    Parser& operator=(const Parser& parser);
    void    Parse(const char* filename,
                  std::map<int, Node>& hosts,
                  std::map<std::pair<int, int>, Node>& services);
  };
}

#endif /* !CORRELATION_PARSER_H_ */
