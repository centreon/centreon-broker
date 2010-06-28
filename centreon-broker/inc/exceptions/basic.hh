/*
** This file is part of Centreon Dhana.
**
** Centreon Dhana is free software: you can redistribute it and/or modify it
** under the terms of the GNU Affero General Public License as published by the
** Free Software Foundation, either version 3 of the License, or (at your
** option) any later version.
**
** Centreon Dhana is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public
** License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with Centreon Dhana. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EXCEPTIONS_BASIC_HH_
# define EXCEPTIONS_BASIC_HH_

# include <exception>
# include "misc/stringifier.hh"

namespace                     exceptions
{
  /**
   *  @class basic basic.hh "exceptions/basic.hh"
   *  @brief Base class of exceptions thrown in Centreon Dhana.
   *
   *  basic is a simple exception class that is only associated with a message.
   *  basic accepts multiple types on input but its limitation is that it only
   *  accepts until basic::max_msg_len characters of exception message.
   */
  class                       basic : private misc::stringifier,
                                      public std::exception
  {
   public:
    static unsigned int const max_msg_len = misc::stringifier::max_len;
                              basic() throw ();
                              basic(basic const& b) throw ();
    virtual                   ~basic() throw ();
    basic&                    operator=(basic const& b) throw ();
    basic&                    operator<<(bool b) throw ();
    basic&                    operator<<(double d) throw ();
    basic&                    operator<<(int i) throw ();
    basic&                    operator<<(unsigned int i) throw ();
    basic&                    operator<<(char const* str) throw ();
    char const*               what() const throw ();
  };
}

#endif /* !EXCEPTIONS_BASIC_HH_ */
