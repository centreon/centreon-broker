/*
** Copyright 2012-2013 Centreon
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

#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/monitoring_logger.hh"
#include "com/centreon/engine/logging.hh"

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
