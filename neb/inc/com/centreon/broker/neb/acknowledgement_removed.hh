/*
** Copyright 2009-2012,2015 Merethis
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

#ifndef CCB_NEB_ACKNOWLEDGEMENT_REMOVED_HH
#  define CCB_NEB_ACKNOWLEDGEMENT_REMOVED_HH

#  include <QString>
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
   *  @class acknowledgement_removed acknowledgement_removed.hh "com/centreon/broker/neb/acknowledgement_removed.hh"
   *  @brief Represents an acknowledgement removed.
   *
   *  Used to notify other modules of the manual removal of acknowledgement.
   */
  class                 acknowledgement_removed : public io::data {
  public:
                        acknowledgement_removed();
                        acknowledgement_removed(
                          acknowledgement_removed const& other);
                        ~acknowledgement_removed();
    acknowledgement_removed&
                        operator=(acknowledgement_removed const& other);
    unsigned int        type() const;

    /**
     *  Get the type of this event.
     *
     *  @return  The event type.
     */
    static unsigned int static_type() {
      return (io::events::data_type<
                io::events::neb,
                neb::de_acknowledgement_removed>::value);
    }

    unsigned int        host_id;
    unsigned int        service_id;

    static mapping::entry const
                        entries[];
    static io::event_info::event_operations const
                        operations;

  private:
    void                _internal_copy(
                          acknowledgement_removed const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_ACKNOWLEDGEMENT_REMOVED_HH
