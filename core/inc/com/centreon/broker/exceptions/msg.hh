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

#ifndef CCB_EXCEPTIONS_MSG_HH_
# define CCB_EXCEPTIONS_MSG_HH_

# include <exception>
# include <time.h>
# include "com/centreon/broker/misc/stringifier.hh"

namespace                           com {
  namespace                         centreon {
    namespace                       broker {
      namespace                     exceptions {
        /**
         *  @class msg msg.hh "exceptions/msg.hh"
         *  @brief Base class of exceptions thrown in Centreon Broker.
         *
         *  msg is a simple exception class that is only associated
         *  with a message. msg accepts multiple types on input but
         *  its limitation is that it only accepts until
         *  msg::max_msg_len characters of exception message.
         */
        class                       msg : private misc::stringifier,
                                          public std::exception {
         public:
          static unsigned int const max_msg_len = misc::stringifier::max_len;
                                    msg() throw ();
                                    msg(msg const& b) throw ();
          virtual                   ~msg() throw ();
          msg&                      operator=(msg const& b) throw ();
          msg&                      operator<<(bool b) throw ();
          msg&                      operator<<(double d) throw ();
          msg&                      operator<<(int i) throw ();
          msg&                      operator<<(unsigned int i) throw ();
          msg&                      operator<<(unsigned long long l) throw ();
          msg&                      operator<<(char const* str) throw ();
          msg&                      operator<<(time_t t) throw ();
          char const*               what() const throw ();
        };
      }
    }
  }
}

#endif /* !CCB_EXCEPTIONS_MSG_HH_ */
