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

#ifndef CCB_NEB_CEOF_PARSER_HH
#  define CCB_NEB_CEOF_PARSER_HH

#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/ceof_iterator.hh"
#  include "com/centreon/broker/neb/ceof_token.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class ceof_parser ceof_parser.hh "com/centreon/broker/neb/ceof_parser.hh"
   *  @brief Centreon Engine Object File parser.
   */
  class          ceof_parser {
  public:
                 ceof_parser(std::string const& str);
                 ~ceof_parser() throw();

   ceof_iterator parse();

  private:   
    std::string  _string;

    std::vector<ceof_token>
                 _tokens;

                 ceof_parser(ceof_parser const& other);
    ceof_parser& operator=(ceof_parser const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_PARSER_HH
