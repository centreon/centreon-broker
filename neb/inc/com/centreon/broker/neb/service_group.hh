/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NEB_SERVICE_GROUP_HH
#  define CCB_NEB_SERVICE_GROUP_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/group.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class service_group service_group.hh "com/centreon/broker/neb/service_group.hh"
   *  @brief Represents a group of services.
   *
   *  Group of services within the scheduling engine.
   */
  class            service_group : public group {
  public:
                   service_group();
                   service_group(service_group const& sg);
                   ~service_group();
    service_group& operator=(service_group const& sg);
    unsigned int   type() const;
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_GROUP_HH
