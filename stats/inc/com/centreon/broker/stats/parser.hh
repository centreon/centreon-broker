/*
** Copyright 2013 Merethis
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

#ifndef CCB_STATS_PARSER_HH
#  define CCB_STATS_PARSER_HH

#  include <map>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   stats {
  // Forward declaration.
  class     config;

  /**
   *  @class parser parser.hh "com/centreon/broker/stats/parser.hh"
   *  @brief Parse a <stats> node.
   *
   *  Parse the <stats> node.
   */
  class     parser {
  public:
            parser();
            parser(parser const& right);
            ~parser() throw ();
    parser& operator=(parser const& right);
    void    parse(
              config& cfg,
              std::string const& content);
  };
}

CCB_END()

#endif // !CCB_STATS_PARSER_HH
