/*
** Copyright 2011 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
