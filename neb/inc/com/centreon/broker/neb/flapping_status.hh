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

#ifndef CCB_NEB_FLAPPING_STATUS_HH
#  define CCB_NEB_FLAPPING_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace            neb {
  /**
   *  @class flapping_status flapping_status.hh "com/centreon/broker/neb/flapping_status.hh"
   *  @brief Store a flapping status.
   *
   *  Store flapping statuses.
   */
  class              flapping_status : public io::data {
  public:
                     flapping_status();
                     flapping_status(flapping_status const& fs);
                     ~flapping_status();
    flapping_status& operator=(flapping_status const& fs);
    unsigned int     type() const;

    timestamp        comment_time;
    timestamp        event_time;
    int              event_type;
    short            flapping_type;
    double           high_threshold;
    unsigned int     host_id;
    unsigned int     internal_comment_id;
    double           low_threshold;
    double           percent_state_change;
    short            reason_type;
    unsigned int     service_id;

  private:
    void             _internal_copy(flapping_status const& fs);
  };
}

CCB_END()

#endif // !CCB_NEB_FLAPPING_STATUS_HH
