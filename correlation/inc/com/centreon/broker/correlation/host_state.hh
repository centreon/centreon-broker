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

#ifndef CCB_CORRELATION_HOST_STATE_HH
#  define CCB_CORRELATION_HOST_STATE_HH

#  include "com/centreon/broker/correlation/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class host_state host_state.hh "com/centreon/broker/correlation/host_state.hh"
   *  @brief Host state.
   *
   *  State of an host at a given time.
   */
  class            host_state : public state {
  public:
                   host_state();
                   host_state(host_state const& hs);
                   ~host_state();
    host_state&    operator=(host_state const& hs);
    bool           operator==(host_state const& hs) const;
    bool           operator!=(host_state const& hs) const;
    unsigned int   type() const;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_HOST_STATE_HH
