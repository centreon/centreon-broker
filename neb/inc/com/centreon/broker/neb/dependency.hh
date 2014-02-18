/*
** Copyright 2009-2013 Merethis
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

#ifndef CCB_NEB_DEPENDENCY_HH
#  define CCB_NEB_DEPENDENCY_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        neb {
  /**
   *  @class dependency dependency.hh "com/centreon/broker/neb/dependency.hh"
   *  @brief Dependency relationship.
   *
   *  Defines a dependency between two objects.
   *
   *  @see host_dependency
   *  @see service_dependency
   */
  class          dependency : public io::data {
  public:
                 dependency();
                 dependency(dependency const& dep);
    virtual      ~dependency();
    dependency&  operator=(dependency const& dep);

    QString      dependency_period;
    unsigned int dependent_host_id;
    bool         enabled;
    QString      execution_failure_options;
    unsigned int host_id;
    bool         inherits_parent;
    QString      notification_failure_options;

  private:
    void         _internal_copy(dependency const& dep);
  };
}

CCB_END()

#endif // !CCB_NEB_DEPENDENCY_HH
