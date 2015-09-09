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

#ifndef CCB_CORRELATION_STATE_HH
#  define CCB_CORRELATION_STATE_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace        correlation {
  /**
   *  @class state state.hh "com/centreon/broker/correlation/state.hh"
   *  @brief State of a checkpoint.
   *
   *  This class represent the state of a specific checkpoint for
   *  a given time.
   */
  class          state : public io::data {
  public:
                 state();
                 state(state const& s);
    virtual      ~state();
    state&       operator=(state const& s);

    timestamp    ack_time;
    int          current_state;
    timestamp    end_time;
    unsigned int host_id;
    unsigned int instance_id;
    bool         in_downtime;
    unsigned int service_id;
    timestamp    start_time;

  private:
    void         _internal_copy(state const& s);
  };
}

CCB_END()

#endif // !CCB_CORRELATION_STATE_HH
