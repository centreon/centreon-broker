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

#ifndef CCB_NOTIFICATION_NODE_HH
#  define CCB_NOTIFICATION_NODE_HH

#  include <ctime>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        notification {

  class     node {
  public:
            node();
            node(node const& obj);
            node& operator=(node const& obj);

    int     get_notification_number() const throw();
    void    set_notification_number(int value);
    time_t  get_last_notification_time() const throw();
    void    set_last_notification_time(time_t value);
    short   get_hard_state() const throw();
    void    set_hard_state(short value);
    short   get_soft_state() const throw();
    void    set_soft_state(short value);

  private:
    int     _notification_number;
    time_t  _last_notification_time;
    short   _hard_state;
    short   _soft_state;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_HH
