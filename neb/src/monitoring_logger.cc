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

#include "com/centreon/broker/neb/monitoring_logger.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/host.hh"
#include "com/centreon/engine/logging.hh"
#include "com/centreon/engine/logging/logger.hh"
#include "com/centreon/engine/service.hh"

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
    : logging::backend(ml) {}

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
monitoring_logger& monitoring_logger::operator=(monitoring_logger const& ml) {
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
void monitoring_logger::log_msg(char const* msg,
                                unsigned int len,
                                logging::type log_type,
                                logging::level l) throw() {
  (void)len;
  if (log_type == logging::debug_type) {
    engine::logging::verbosity_level verbosity;
    switch (l) {
      case logging::low:
        verbosity = engine::logging::most;
        break;
      case logging::medium:
        verbosity = engine::logging::more;
        break;
      default:
        verbosity = engine::logging::basic;
    };
    logger(engine::logging::dbg_all, verbosity) << "Centreon Broker: " << msg;
  } else {
    bool display;
    engine::logging::type_value data_type;
    switch (log_type) {
      case logging::config_type:
        display = false;
        data_type = engine::logging::log_config_warning;
        break;
      case logging::info_type:
        display = false;
        data_type = engine::logging::log_process_info;
        break;
      default:
        display = true;
        data_type = engine::logging::log_runtime_error;
    };
    logger(data_type,
           (display ? engine::logging::basic : engine::logging::more))
        << "Centreon Broker: " << msg;
  }
  return;
}
