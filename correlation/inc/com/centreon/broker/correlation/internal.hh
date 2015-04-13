/*
** Copyright 2013 Merethis
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

#ifndef CCB_CORRELATION_INTERNAL_HH
#  define CCB_CORRELATION_INTERNAL_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace correlation {
  // Data elements.
  enum data_element {
    de_engine_state = 1,
    de_host_state,
    de_issue,
    de_issue_parent,
    de_service_state,
    de_state,
    de_log_issue
  };

  extern unsigned int instance_id;
}

CCB_END()

#endif // !CCB_CORRELATION_INTERNAL_HH
