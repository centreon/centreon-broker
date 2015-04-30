/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_BBDO_VERSION_RESPONSE_HH
#  define CCB_BBDO_VERSION_RESPONSE_HH

#  include <QString>
#  include "com/centreon/broker/bbdo/internal.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/io/events.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               bbdo {
  /**
   *  @class version_response version_response.hh "com/centreon/broker/bbdo/version_response.hh"
   *  @brief Send protocol version used by endpoint.
   *
   *  Send protocol version used by endpoint.
   */
  class                 version_response : public io::data {
  public:
                        version_response();
                        version_response(version_response const& other);
                        ~version_response();
    version_response&   operator=(version_response const& other);
    unsigned int        type() const;

    /**
     *  Get the event type.
     *
     *  @return The event type.
     */
    static unsigned int static_type() {
      return (io::events::data_type<
                            io::events::bbdo,
                            bbdo::de_version_response>::value);
    }

    short               bbdo_major;
    short               bbdo_minor;
    short               bbdo_patch;
    QString             extensions;

    static mapping::entry const
                        entries[];
    static io::event_info::event_operations const
                        operations;

  private:
    void                _internal_copy(version_response const& right);
  };
}

CCB_END()

#endif // !CCB_BBDO_VERSION_RESPONSE_HH
