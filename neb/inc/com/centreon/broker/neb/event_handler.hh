/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_EVENT_HANDLER_HH
#  define CCB_NEB_EVENT_HANDLER_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class event_handler event_handler.hh "com/centreon/broker/neb/event_handler.hh"
   *  @brief Represents an event handler inside the scheduling engine.
   *
   *  Event handlers, as their name suggests, are executed upon
   *  the detection of some events by the scheduling engine.
   */
  class            event_handler : public io::data {
  public:
                   event_handler();
                   event_handler(event_handler const& eh);
                   ~event_handler();
    event_handler& operator=(event_handler const& eh);
    QString const& type() const;

    QString        command_args;
    QString        command_line;
    short          early_timeout;
    timestamp      end_time;
    double         execution_time;
    short          handler_type;
    unsigned int   host_id;
    QString        output;
    short          return_code;
    unsigned int   service_id;
    timestamp      start_time;
    short          state;
    short          state_type;
    short          timeout;

  private:
    void           _internal_copy(event_handler const& eh);
  };
}

CCB_END()

#endif // !CCB_NEB_EVENT_HANDLER_HH
