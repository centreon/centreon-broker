/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_SERVICE_DEPENDENCY_HH
#  define CCB_NEB_SERVICE_DEPENDENCY_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/dependency.hh"

CCB_BEGIN()

namespace               neb {
  /**
   *  @class service_dependency service_dependency.hh "com/centreon/broker/neb/service_dependency.hh"
   *  @brief Service dependency relationship.
   *
   *  Defines a dependency between two services.
   */
  class                 service_dependency : public dependency {
  public:
                        service_dependency();
                        service_dependency(
                          service_dependency const& sd);
                        ~service_dependency();
    service_dependency& operator=(service_dependency const& sd);
    QString const&      type() const;

    unsigned int        dependent_service_id;
    unsigned int        service_id;

  private:
    void                _internal_copy(service_dependency const& sd);
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_DEPENDENCY_HH
