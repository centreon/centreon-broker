/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_NOTIFICATION_METHOD_HH
#  define CCB_NOTIFICATION_NOTIFICATION_METHOD_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace         notification {
  namespace       objects {
    /**
     *  @class notification_method notification_method.hh "com/centreon/broker/notification/objects/notification_method.hh"
     *  @brief The notification method object.
     *
     */
    class           notification_method {
    public:
                    DECLARE_SHARED_PTR(notification_method);

                    notification_method();
                    notification_method(notification_method const& obj);
      notification_method&
                    operator=(notification_method const& obj);

      std::string const&
                    get_name() const throw();
      void          set_name(std::string const& val);

      unsigned int  get_command_id() const throw();
      void          set_command_id(unsigned int id) throw();

      unsigned int  get_interval() const throw();
      void          set_interval(unsigned int val) throw();

      std::string const&
                    get_status() const throw();
      void          set_status(std::string const& val);

      std::string const&
                    get_types() const throw();
      void          set_types(std::string const& val);

      unsigned int  get_start() const throw();
      void          set_start(unsigned int val) throw();

      unsigned int  get_end() const throw();
      void          set_end(unsigned int val) throw();

    private:
      std::string   _name;
      unsigned int  _command_id;
      unsigned int  _interval;
      std::string   _status;
      std::string   _types;
      unsigned int  _start;
      unsigned int  _end;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_NOTIFICATION_METHOD_HH
