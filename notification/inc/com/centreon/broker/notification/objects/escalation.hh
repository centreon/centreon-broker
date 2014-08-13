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

#ifndef CCB_NOTIFICATION_ESCALATION_HH
#  define CCB_NOTIFICATION_ESCALATION_HH

#  include <string>
#  include "com/centreon/broker/notification/objects/group.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       notification {

  class         escalation {
  public:
    enum              type {
      unknown = 0,
      host = 1,
      service = 2
    };

    enum              action_on {
      none = 0,
      service_unknown = (1 << 1),
      service_warning = (1 << 2),
      service_critical = (1 << 3),
      service_pending = (1 << 4),
      service_recovery = (1 << 5),
      host_down = (1 << 6),
      host_unreachable = (1 << 7)
    };

    escalation();
    escalation(escalation const& obj);
    escalation& operator=(escalation const& obj);

    bool              is_host_escalation() const throw();
    bool              is_service_escalation() const throw();
    void              set_is_host_escalation(bool val) throw();
    void              set_is_service_escalation(bool val) throw();

    group const&      get_contactgroups() const throw();
    void              set_contactgroups(group const& val);
    group const&      get_contacts() const throw();
    void              set_contacts(group const& val);
    unsigned short    get_escalation_options() const throw();
    void              set_escalation_options(unsigned int val) throw();
    std::string const& get_escalation_period() const throw();
    void              set_escalation_period(const std::string& val);
    unsigned int      get_first_notification() const throw();
    void              set_first_notification(unsigned int val) throw();
    group const&      get_hostgroups() const throw();
    void              set_hostgroups(group const& val);
    group const&      get_hosts() const throw();
    void              set_hosts(group const& val);
    unsigned int      get_last_notification() const throw();
    void              set_last_notification(unsigned int val) throw();
    unsigned int      get_notification_interval() const throw();
    void              set_notification_interval(unsigned int val) throw();
    group const&      get_servicegroups() const throw();
    void              set_servicegroups(group const& val);

  private:
    type              _type;

    group             _contactgroups;
    group             _contacts;
    unsigned short    _escalation_options;
    std::string       _escalation_period;
    unsigned int      _first_notification;
    group             _hostgroups;
    group             _hosts;
    unsigned int      _last_notification;
    unsigned int      _notification_interval;
    group             _servicegroups;
    group             _service_description;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_ESCALATION_HH
