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
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace             notification {
  /**
   *  @class escalation escalation.hh "com/centreon/broker/notification/objects/escalation.hh"
   *  @brief Escalation object.
   *
   *  The object containing an escalation.
   */
  class               escalation {
  public:
                      DECLARE_SHARED_PTR(escalation);

    enum              type {
      unknown = 0,
      host = (1 << 1),
      service = (1 << 2),
      hostgroup = (1 << 3),
      servicegroup = (1 << 4)
    };

    enum              action_on {
      none = 0,
      service_unknown = (1 << 1),
      service_warning = (1 << 2),
      service_critical = (1 << 3),
      service_pending = (1 << 4),
      service_recovery = (1 << 5),
      host_down = (1 << 6),
      host_unreachable = (1 << 7),
      host_recovery = (1 << 8)
    };

                      escalation();
                      escalation(escalation const& obj);
                      escalation& operator=(escalation const& obj);

    void              set_types(type t) throw();
    void              set_type(type t) throw();
    bool              is_type(type t) const throw();

    action_on         get_escalation_options() const throw();
    void              set_escalation_options(action_on val) throw();
    void              set_escalation_option(action_on val) throw();
    bool              is_escalation_option_set(action_on val) const throw();
    std::string const&
                      get_escalation_period() const throw();
    void              set_escalation_period(const std::string& val);
    unsigned int      get_first_notification() const throw();
    void              set_first_notification(unsigned int val) throw();
    unsigned int      get_last_notification() const throw();
    void              set_last_notification(unsigned int val) throw();
    unsigned int      get_notification_interval() const throw();
    void              set_notification_interval(unsigned int val) throw();

    void              parse_host_escalation_options(std::string const& line);
    void              parse_service_escalation_options(std::string const& line);

  private:
    type              _type;

    action_on         _escalation_options;
    std::string       _escalation_period;
    unsigned int      _first_notification;
    unsigned int      _last_notification;
    unsigned int      _notification_interval;

    struct            name_to_action {
      const char* name;
      action_on action;
    };

    static const name_to_action
                      _service_actions[];
    static const name_to_action
                      _host_actions[];
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_ESCALATION_HH
