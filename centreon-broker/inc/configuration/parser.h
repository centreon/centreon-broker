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

#ifndef CONFIGURATION_PARSER_H_
# define CONFIGURATION_PARSER_H_

# include <list>

namespace   Configuration
{
  // Forward declaration.
  class     Interface;
  class     Log;

  /**
   *  \class Parser parser.h "configuration/parser.h"
   *  \brief Parse a configuration file.
   *
   *  Parse a configuration file and generate proper configuration objects.
   */
  class     Parser
  {
   private:
            Parser(const Parser& parser);
    Parser& operator=(const Parser& parser);

   public:
            Parser();
            ~Parser();
    void    Parse(const std::string& filename,
                  std::list<Interface>& inputs,
                  std::list<Log>& logs,
                  std::list<Interface>& outputs);
  };
}

#endif /* !CONFIGURATION_PARSER_H_ */
