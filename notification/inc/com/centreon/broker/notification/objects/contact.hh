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

#ifndef CCB_NOTIFICATION_CONTACT_HH
#  define CCB_NOTIFICATION_CONTACT_HH

#  include <map>
#  include <vector>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       notification {

  class         contact {
  public:
    contact();
    contact(contact const& obj);
    contact& operator=(contact const& obj);

    std::vector<std::string>& get_address() const throw();
    void                     add_address(std::string&);



  private:
    std::vector<std::string> _address;
    std::string              _alias;
    bool                     _can_submit_commands;
    group                    _contactgroups;
    std::string              _contact_name;
    std::map<std::string, std::string>
                             _customvariables;
    std::string              _email;
    bool                     _host_notifications_enabled;
    group                    _host_notification_commands;
    unsigned int             _host_notification_options;
    std::string              _host_notification_period;
    bool                     _retain_nonstatus_information;
    bool                     _retain_status_information;
    std::string              _pager;
    group                    _service_notification_commands;
    unsigned int             _service_notification_options;
    std::string              _service_notification_period;
    bool                     _service_notifications_enabled;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_CONTACT_HH
