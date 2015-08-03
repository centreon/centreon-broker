/*
** Copyright 2015 Merethis
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

#ifndef CCB_NEB_CEOF_TOKEN_HH
#  define CCB_NEB_CEOF_TOKEN_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        neb {
  /**
   *  @class ceof_token ceof_token.hh "com/centreon/broker/neb/ceof_token.hh"
   *  @brief Centreon Engine Object File token.
   *
   *  As produced by the parser.
   */
  class          ceof_token {
  public:
    enum         token_type {
                 object,
                 key,
                 value
    };

                 ceof_token();
                 ceof_token(
                   token_type type,
                   std::string const& val,
                   int token_number,
                   int parent);
                 ceof_token(ceof_token const& other);
    ceof_token&  operator=(ceof_token const& other);
                 ~ceof_token() throw ();

    token_type   get_type() const throw ();
    std::string const&
                 get_value() const throw ();
    int          get_token_number() const throw ();
    int          get_parent_token() const throw ();

  private:
    token_type   _type;
    std::string  _value;
    int          _token_number;
    int          _parent_token;
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_TOKEN_HH
