/*
** Copyright 2009-2011 Centreon
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
