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
#  include "com/centreon/broker/notification/objects/group.hh"

CCB_BEGIN()

namespace       notification {

  class         contact {
  public:
    contact();
    contact(contact const& obj);
    contact& operator=(contact const& obj);

    std::vector<std::string> const& get_address() const throw();
    void                     add_address(std::string const& value);
    std::string const&       get_alias() const throw();
    void                     set_alias(std::string const& value);
    bool                     get_can_submit_commands() const throw();
    void                     set_can_submit_commands(bool value) throw();
    group const&             get_group() const throw();
    void                     set_group(group const& value);
    std::map<std::string, std::string> const&
                             get_customvariables() const throw();
    void                     add_customvariables(std::string const& name,
                                                 std::string const& value);
    std::string const&       get_email() const throw();
    void                     set_email(std::string const& value);
    bool                     get_host_notifications_enabled() const throw();
    void                     set_host_notifications_enabled(bool value) throw();
    group const&             get_host_notification_commands() const throw();
    void                     set_host_notification_commands(group const& value);
    unsigned int             get_host_notification_options() const throw();
    void                     set_host_notification_options(unsigned int value) throw();
    std::string const&       get_host_notification_period() const throw();
    void                     set_host_notification_period(std::string const& value);
    bool                     get_retain_nonstatus_information() const throw();
    void                     set_retain_nonstatus_information(bool value) throw();
    bool                     get_retain_status_information() const throw();
    void                     set_retain_status_information(bool value) throw();
    std::string const&       get_pager() const throw();
    void                     set_pager(std::string const& value);
    group const&              get_service_notification_commands() const throw();
    void                     set_service_notification_commands(group const& value);
    unsigned int             get_service_notification_options() const throw();
    void                     set_service_notification_options(unsigned int value) throw();
    std::string const&       get_service_notification_period() const throw();
    void                     set_service_notification_period(std::string const&);
    bool                     get_service_notifications_enabled() const throw();
    void                     set_service_notifications_enabled(bool value) throw();

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
