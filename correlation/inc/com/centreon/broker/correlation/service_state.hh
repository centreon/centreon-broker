/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_CORRELATION_SERVICE_STATE_HH
#  define CCB_CORRELATION_SERVICE_STATE_HH

#  include "com/centreon/broker/correlation/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class service_state service_state.hh "com/centreon/broker/correlation/service_state.hh"
   *  @brief Service state.
   *
   *  State of a service at a given time.
   */
  class            service_state : public state {
   public:
                   service_state();
                   service_state(service_state const& ss);
                   ~service_state();
    service_state& operator=(service_state const& ss);
    bool           operator==(service_state const& ss) const;
    bool           operator!=(service_state const& ss) const;
    unsigned int   type() const;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_SERVICE_STATE_HH
