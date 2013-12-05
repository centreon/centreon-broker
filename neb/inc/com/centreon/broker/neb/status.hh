/*
** Copyright 2009-2013 Merethis
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

#ifndef CCB_NEB_STATUS_HH
#  define CCB_NEB_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        neb {
  /**
   *  @class status status.hh "com/centreon/broker/neb/status.hh"
   *  @brief Root class of status events.
   *
   *  This is the root class of status events : host, program and
   *  service status events.
   *
   *  @see host_status
   *  @see program_status
   *  @see service_status
   */
  class          status : public io::data {
  public:
                 status();
                 status(status const& s);
    virtual      ~status();
    status&      operator=(status const& s);

    bool         event_handler_enabled;
    bool         failure_prediction_enabled;
    bool         flap_detection_enabled;
    unsigned int instance_id;
    bool         notifications_enabled;
    bool         process_performance_data;

  private:
    void         _internal_copy(status const& s);
  };
}

CCB_END()

#endif // !CCB_NEB_STATUS_HH
