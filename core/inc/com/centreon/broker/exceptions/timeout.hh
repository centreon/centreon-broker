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

#ifndef CCB_EXCEPTIONS_TIMEOUT_HH
#  define CCB_EXCEPTIONS_TIMEOUT_HH

#  include <exception>
#  include "com/centreon/broker/misc/stringifier.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               exceptions {
  /**
   *  @class timeout timeout.hh "com/centreon/broker/exceptions/timeout.hh"
   *  @brief Timeout exception.
   *
   *  Exception that is thrown upon timeout.
   */
  class                 timeout : private misc::stringifier,
                                  public std::exception {
  public:
                        timeout();
                        timeout(timeout const& other);
    virtual             ~timeout() throw ();
    timeout&            operator=(timeout const& other);
    virtual timeout*    clone() const;
    virtual void        rethrow() const;
    virtual char const* what() const throw ();

    /**
     *  Insert data in message.
     *
     *  @param[in] t  Data to insert.
     */
    template            <typename T>
    timeout&            operator<<(T t) {
      misc::stringifier::operator<<(t);
      return (*this);
    }
  };
}

CCB_END()

#endif // !CCB_EXCEPTIONS_TIMEOUT_HH
