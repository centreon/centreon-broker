/*
** Copyright 2011 Merethis
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

#ifndef CCB_IO_EXCEPTIONS_SHUTDOWN_HH_
# define CCB_IO_EXCEPTIONS_SHUTDOWN_HH_

# include "com/centreon/broker/exceptions/msg.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       io {
  namespace     exceptions {
    /**
     *  @class shutdown shutdown.hh "com/centreon/broker/io/exceptions/shutdown.hh"
     *  @brief Shutdown exception class.
     *
     *  This exception is thrown when someone attemps to read from a
     *  stream that has been shutdown.
     */
    class       shutdown : public com::centreon::broker::exceptions::msg {
     private:
      bool      _in_shutdown;
      bool      _out_shutdown;
      void      _internal_copy(shutdown const& s);

     public:
                shutdown(bool in_shutdown, bool out_shutdown);
                shutdown(shutdown const& s);
                ~shutdown() throw ();
      shutdown& operator=(shutdown const& s);
      msg*      clone() const;
      bool      is_in_shutdown() const;
      bool      is_out_shutdown() const;
      void      rethrow() const;

      /**
       *  Insert data in message.
       *
       *  @param[in] t Data to insert.
       */
      template  <typename T>
      shutdown& operator<<(T t) throw () {
        com::centreon::broker::exceptions::msg::operator<<(t);
        return (*this);
      }
    };
  }
}

CCB_END()

#endif /* !CCB_IO_EXCEPTIONS_SHUTDOWN_HH_ */
