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

#ifndef CCB_NEB_ACKNOWLEDGEMENT_HH
#  define CCB_NEB_ACKNOWLEDGEMENT_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/io/events.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/internal.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace               neb {
  /**
   *  @class acknowledgement acknowledgement.hh "com/centreon/broker/neb/acknowledgement.hh"
   *  @brief Represents an acknowledgement inside Nagios.
   *
   *  When some service or host is critical, Centreon Engine will emit
   *  notifications according to its configuration. To stop the
   *  notification process, a user can acknowledge the problem.
   */
  class                 acknowledgement : public io::data {
  public:
                        acknowledgement();
                        acknowledgement(acknowledgement const& other);
                        ~acknowledgement();
    acknowledgement&    operator=(acknowledgement const& other);
    unsigned int        type() const;

    /**
     *  Get the type of this event.
     *
     *  @return  The event type.
     */
    static unsigned int static_type() {
      return (io::events::data_type<
                            io::events::neb,
                            neb::de_acknowledgement>::value);
    }

    short               acknowledgement_type;
    std::string             author;
    std::string             comment;
    timestamp           deletion_time;
    timestamp           entry_time;
    unsigned int        host_id;
    bool                is_sticky;
    bool                notify_contacts;
    bool                notify_only_if_not_already_acknowledged;
    bool                persistent_comment;
    unsigned int        poller_id;
    unsigned int        service_id;
    short               state;

    static mapping::entry const
                        entries[];
    static io::event_info::event_operations const
                        operations;

  private:
    void                _internal_copy(acknowledgement const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_ACKNOWLEDGEMENT_HH
