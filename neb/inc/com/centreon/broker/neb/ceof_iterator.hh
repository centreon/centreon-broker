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

#ifndef CCB_NEB_CEOF_ITERATOR_HH
#  define CCB_NEB_CEOF_ITERATOR_HH

#  include <vector>
#  include "com/centreon/broker/neb/ceof_token.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class ceof_iterator ceof_iterator.hh "com/centreon/broker/neb/ceof_iterator.hh"
   *  @brief Centreon Engine Object File iterator.
   *
   *  This iterates over the parsed tokens of a ceof document.
   */
  class          ceof_iterator {
  public:
                 ceof_iterator();
                 ceof_iterator(
                   std::vector<ceof_token>::const_iterator const& begin,
                   std::vector<ceof_token>::const_iterator const& end);
                 ceof_iterator(ceof_iterator const& other);
   ceof_iterator&
                 operator=(ceof_iterator const& other);
                 ~ceof_iterator() throw();
   bool          operator==(ceof_iterator const& other) const throw();
   bool          operator!=(ceof_iterator const& other) const throw();
   ceof_iterator& operator++() throw();

   ceof_token::token_type
                 get_type() const throw();
   std::string const&
                 get_value() const throw();

   bool          has_children() const throw();
   ceof_iterator enter_children() const throw();

   bool          end() const throw();

  private:
   std::vector<ceof_token>::const_iterator
                 _token_it;
   std::vector<ceof_token>::const_iterator
                 _token_end;
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_ITERATOR_HH
