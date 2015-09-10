/*
** Copyright 2009-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
    unsigned int        type() const;

    unsigned int        dependent_service_id;
    unsigned int        service_id;

  private:
    void                _internal_copy(service_dependency const& sd);
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_DEPENDENCY_HH
