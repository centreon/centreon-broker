/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_BUILDERS_ESCALATION_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_ESCALATION_BUILDER_HH

#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/escalation.hh"

CCB_BEGIN()

namespace       notification {

  class         escalation_builder {
  public:
    virtual ~escalation_builder() {}

    virtual void add_escalation(unsigned int id,
                                shared_ptr<escalation> esc) = 0;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_ESCALATION_BUILDER_HH
