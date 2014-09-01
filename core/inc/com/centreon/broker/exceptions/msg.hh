/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_EXCEPTIONS_MSG_HH_
# define CCB_EXCEPTIONS_MSG_HH_

# include <exception>
# include "com/centreon/broker/misc/stringifier.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               exceptions {
  /**
   *  @class msg msg.hh "com/centreon/broker/exceptions/msg.hh"
   *  @brief Base class of exceptions thrown in Centreon Broker.
   *
   *  msg is a simple exception class that is only associated with a
   *  message. msg accepts multiple types on input.
   */
  class                 msg : private misc::stringifier,
			      public std::exception {
   public:
                        msg() throw ();
                        msg(msg const& b) throw ();
    virtual             ~msg() throw ();
    msg&                operator=(msg const& b) throw ();
    virtual msg*        clone() const;
    virtual void        rethrow() const;
    virtual char const* what() const throw ();

    /**
     *  Insert data in message.
     *
     *  @param[in] t Data to insert.
     */
    template            <typename T>
    msg&                operator<<(T t) throw () {
      misc::stringifier::operator<<(t);
      return (*this);
    }
  };
}

CCB_END()

#endif /* !CCB_EXCEPTIONS_MSG_HH_ */
