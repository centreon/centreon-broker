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
#  include "com/centreon/broker/notification/objects/defines.hh"
#  include "com/centreon/broker/notification/objects/group.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace   notification {
  namespace objects {
    /**
     *  @class contact contact.hh "com/centreon/broker/notification/objects/contact.hh"
     *  @brief Contact object.
     *
     *  The object containing a contact.
     */
    class                      contact {
    public:
                               DECLARE_SHARED_PTR(contact);

      enum                     host_action_on {
        host_none = 0,
        host_up = (1 << 0),
        host_down = (1 << 1),
        host_unreachable = (1 << 2),
        host_flapping = (1 << 3),
        host_downtime = (1 << 4)
      };

      enum                     service_action_on {
        service_none = 0,
        service_ok = (1 << 0),
        service_warning = (1 << 1),
        service_unknown = (1 << 2),
        service_critical = (1 << 3),
        service_flapping = (1 << 4),
        service_downtime = (1 << 5)
      };

                               contact();
                               contact(contact const& obj);
                               contact& operator=(contact const& obj);

      std::vector<std::string> const& get_address() const throw();
      void                     add_address(std::string const& value);
      std::string const&       get_alias() const throw();
      void                     set_alias(std::string const& value);
      bool                     get_can_submit_commands() const throw();
      void                     set_can_submit_commands(bool value) throw();
      std::string const&       get_name() const throw();
      void                     set_name(std::string const& value);
      std::map<std::string, std::string> const&
                               get_customvariables() const throw();
      void                     add_customvariables(std::string const& name,
                                                   std::string const& value);
      std::string const&       get_email() const throw();
      void                     set_email(std::string const& value);
      bool                     get_host_notifications_enabled() const throw();
      void                     set_host_notifications_enabled(bool value) throw();
      host_action_on           get_host_notification_options() const throw();
      void                     set_host_notification_options(
                                  host_action_on value) throw();
      void                     set_host_notification_option(
                                  host_action_on value) throw();
      bool                     can_be_notified_for_host(
                                  host_action_on value) const throw();
      std::string const&       get_host_notification_period() const throw();
      void                     set_host_notification_period(
                                  std::string const& value);
      bool                     get_retain_nonstatus_information() const throw();
      void                     set_retain_nonstatus_information(
                                  bool value) throw();
      bool                     get_retain_status_information() const throw();
      void                     set_retain_status_information(bool value) throw();
      std::string const&       get_pager() const throw();
      void                     set_pager(std::string const& value);
      service_action_on        get_service_notification_options() const throw();
      void                     set_service_notification_options(
                                  service_action_on value) throw();
      void                     set_service_notification_option(
                                  service_action_on value) throw();
      bool                     can_be_notified_for_service(
                                  service_action_on value) const throw();
      std::string const&       get_service_notification_period() const throw();
      void                     set_service_notification_period(
                                  std::string const&);
      bool                     get_service_notifications_enabled() const throw();
      void                     set_service_notifications_enabled(
                                  bool value) throw();
      bool                     can_be_notified(node_state state, bool isHost) const throw();

    private:
      std::vector<std::string> _address;
      std::string              _alias;
      bool                     _can_submit_commands;
      std::string              _contact_name;
      std::map<std::string, std::string>
                               _customvariables;
      std::string              _email;
      bool                     _host_notifications_enabled;
      host_action_on           _host_notification_options;
      std::string              _host_notification_period;
      bool                     _retain_nonstatus_information;
      bool                     _retain_status_information;
      std::string              _pager;
      service_action_on        _service_notification_options;
      std::string              _service_notification_period;
      bool                     _service_notifications_enabled;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_CONTACT_HH
