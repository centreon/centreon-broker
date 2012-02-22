/*
** Copyright 2012 Merethis
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

#ifndef CCB_NEB_MONITORING_LOGGER_HH
#  define CCB_NEB_MONITORING_LOGGER_HH

#  include "com/centreon/broker/logging/backend.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              neb {
  /**
   *  @class monitoring_logger monitoring_logger.hh "com/centreon/broker/neb/monitoring_logger.hh"
   *  @brief Log messages to the monitoring engine's log file.
   *
   *  Log messages to the monitoring engine's log file.
   */
  class                monitoring_logger : public logging::backend {
  public:
                       monitoring_logger();
                       monitoring_logger(monitoring_logger const& ml);
                       ~monitoring_logger();
    monitoring_logger& operator=(monitoring_logger const& ml);
    void               log_msg(
                         char const* msg,
                         unsigned int len,
                         logging::type log_type,
                         logging::level l) throw ();

  private:
    void               _internal_copy(monitoring_logger const& ml);
  };
}

CCB_END()

#endif // !CCB_NEB_MONITORING_LOGGER_HH
