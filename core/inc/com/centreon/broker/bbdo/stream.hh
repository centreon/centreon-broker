/*
** Copyright 2013 Centreon
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

#ifndef CCB_BBDO_STREAM_HH
#  define CCB_BBDO_STREAM_HH

#  include <QString>
#  include "com/centreon/broker/bbdo/input.hh"
#  include "com/centreon/broker/bbdo/output.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            bbdo {
  /**
   *  @class stream stream.hh "com/centreon/broker/bbdo/stream.hh"
   *  @brief BBDO stream.
   *
   *  The class converts data to NEB events back and forth.
   */
  class              stream : public input, public output {
  public:
    enum             negociation_type {
      negociate_first = 1,
      negociate_second,
      negociated
    };

                     stream();
                     stream(stream const& other);
                     ~stream();
    stream&          operator=(stream const& other);
    void             negociate(negociation_type neg);
    bool             read(
                       misc::shared_ptr<io::data>& d,
                       time_t deadline = (time_t)-1);
    void             set_ack_limit(unsigned int limit);
    void             set_coarse(bool coarse);
    void             set_negociate(
                       bool negociate,
                       QString const& extensions = QString());
    void             set_timeout(int timeout);
    void             statistics(io::properties& tree) const;
    int              write(misc::shared_ptr<io::data> const& d);
    void             acknowledge_events(unsigned int events);
    void             send_event_acknowledgement();

  private:
    bool             _coarse;
    QString          _extensions;
    bool             _negociate;
    bool             _negociated;
    int              _timeout;
    unsigned int     _acknowledged_events;
    unsigned int     _ack_limit;
    unsigned int     _events_received_since_last_ack;
  };
}

CCB_END()

#endif // !CCB_BBDO_STREAM_HH
