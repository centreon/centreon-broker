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

#ifndef CCB_EXCEPTIONS_CONFIG_HH
#  define CCB_EXCEPTIONS_CONFIG_HH

#  include "com/centreon/broker/exceptions/msg.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace    exceptions {
  /**
   *  @class config config.hh "com/centreon/broker/exceptions/config.hh"
   *  @brief Configuration exception.
   *
   *  Such exceptions are thrown in case of configuration errors.
   */
  class      config : public msg {
  public:
             config();
             config(config const& other);
             ~config() throw ();
    config&  operator=(config const& other);

    /**
     *  Insert data in message.
     *
     *  @param[in] t  Data to insert.
     *
     *  @return This object.
     */
    template <typename T>
    config&  operator<<(T t) {
      msg::operator<<(t);
      return (*this);
    }
  };
}

CCB_END()

#endif // !CCB_EXCEPTIONS_CONFIG_HH
