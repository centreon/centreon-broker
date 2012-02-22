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

#include "com/centreon/broker/neb/monitoring_logger.hh"
#include "nagios/nagios.h"

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
  _internal_copy(ml);
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
    _internal_copy(ml);
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
    int verbosity;
    switch (l) {
    case logging::low:
      verbosity = DEBUGV_MOST;
      break ;
    case logging::medium:
      verbosity = DEBUGV_MORE;
      break ;
    default:
      verbosity = DEBUGV_BASIC;
    };
    log_debug_info(DEBUGL_EVENTBROKER, verbosity, "Centreon Broker: %s", msg);
  }
  else {
    int display;
    int data_type;
    switch (log_type) {
    case logging::config_type:
      display = FALSE;
      data_type = NSLOG_CONFIG_WARNING;
      break ;
    case logging::info_type:
      display = FALSE;
      data_type = NSLOG_INFO_MESSAGE;
      break ;
    default:
      display = TRUE;
      data_type = NSLOG_RUNTIME_ERROR;
    };
    logit(data_type, display, "Centreon Broker: %s", msg);
  }
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal data members.
 *
 *  This class does not contain any data member, so this method does
 *  nothing.
 *
 *  @param[in] ml Object to copy.
 */
void monitoring_logger::_internal_copy(monitoring_logger const& ml) {
  (void)ml;
  return ;
}
