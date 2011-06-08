/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_EXCEPTIONS_BASIC_HH_
# define CCB_EXCEPTIONS_BASIC_HH_

# include <exception>
# include <time.h>
# include "misc/stringifier.hh"

namespace                           com {
  namespace                         centreon {
    namespace                       broker {
      namespace                     exceptions {
        /**
         *  @class basic basic.hh "exceptions/basic.hh"
         *  @brief Base class of exceptions thrown in Centreon Broker.
         *
         *  basic is a simple exception class that is only associated
         *  with a message. basic accepts multiple types on input but
         *  its limitation is that it only accepts until
         *  basic::max_msg_len characters of exception message.
         */
        class                       basic : private misc::stringifier,
                                            public std::exception {
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
          basic&                    operator<<(time_t t) throw ();
          char const*               what() const throw ();
        };
      }
    }
  }
}

#endif /* !CCB_EXCEPTIONS_BASIC_HH_ */
