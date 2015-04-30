/*
** Copyright 2012-2013,2015 Merethis
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

#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/monitoring_logger.hh"
#include "com/centreon/engine/logging/logger.hh"

using namespace com::centreon;
using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
monitoring_logger::monitoring_logger() {}

/**
 *  Copy constructor.
 *
 *  @param[in] ml Object to copy.
 */
monitoring_logger::monitoring_logger(monitoring_logger const& ml)
  : logging::backend(ml) {

}

/**
 *  Destructor.
 */
monitoring_logger::~monitoring_logger() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ml Object to copy.
 *
 *  @return This object.
 */
monitoring_logger& monitoring_logger::operator=(
                                        monitoring_logger const& ml) {
  if (this != &ml) {
    backend::operator=(ml);
  }
  return (*this);
}

/**
 *  Log message in the engine's log.
 *
 *  @param[in] msg Message to log.
 *  @param[in] len Length of msg.
 *  @param[in] log_type Type of log message.
 *  @param[in] l        Verbosity level.
 */
void monitoring_logger::log_msg(
                          char const* msg,
                          unsigned int len,
                          logging::type log_type,
                          logging::level l) throw () {
  (void)len;
  if (log_type == logging::debug_type) {
    engine::logging::verbosity_level verbosity;
    switch (l) {
    case logging::low:
      verbosity = engine::logging::most;
      break ;
    case logging::medium:
      verbosity = engine::logging::more;
      break ;
    default:
      verbosity = engine::logging::basic;
    };
    logger(engine::logging::dbg_all, verbosity)
      << "Centreon Broker: " << msg;
  }
  else {
    bool display;
    engine::logging::type_value data_type;
    switch (log_type) {
    case logging::config_type:
      display = false;
      data_type = engine::logging::log_config_warning;
      break ;
    case logging::info_type:
      display = false;
      data_type = engine::logging::log_process_info;
      break ;
    default:
      display = true;
      data_type = engine::logging::log_runtime_error;
    };
    logger(
      data_type,
      (display ? engine::logging::basic : engine::logging::more))
      << "Centreon Broker: " << msg;
  }
  return ;
}
