/*
** Copyright 2011-2013 Centreon
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

#ifndef CC_LOGGING_LOGGER_HH
#define CC_LOGGING_LOGGER_HH

#include "com/centreon/logging/engine.hh"
#include "com/centreon/logging/temp_logger.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace logging {
enum type_value {
  type_info = (1ull << 61),
  type_debug = (1ull << 62),
  type_error = (1ull << 63)
};

enum verbosity_level { low = 0, medium = 1, high = 2 };
}  // namespace logging

CC_END()

#define log_info(verbose)                                                \
  for (uint32_t __com_centreon_logging_define_ui(0);                 \
       !__com_centreon_logging_define_ui &&                              \
       com::centreon::logging::engine::instance().is_log(                \
           com::centreon::logging::type_info, verbose);                  \
       ++__com_centreon_logging_define_ui)                               \
  com::centreon::logging::temp_logger(com::centreon::logging::type_info, \
                                      verbose)                           \
      << "[info] "

#define log_debug(verbose)                                                \
  for (uint32_t __com_centreon_logging_define_ui(0);                  \
       !__com_centreon_logging_define_ui &&                               \
       com::centreon::logging::engine::instance().is_log(                 \
           com::centreon::logging::type_debug, verbose);                  \
       ++__com_centreon_logging_define_ui)                                \
  com::centreon::logging::temp_logger(com::centreon::logging::type_debug, \
                                      verbose)                            \
      << "[debug] "

#define log_error(verbose)                                                \
  for (uint32_t __com_centreon_logging_define_ui(0);                  \
       !__com_centreon_logging_define_ui &&                               \
       com::centreon::logging::engine::instance().is_log(                 \
           com::centreon::logging::type_error, verbose);                  \
       ++__com_centreon_logging_define_ui)                                \
  com::centreon::logging::temp_logger(com::centreon::logging::type_error, \
                                      verbose)                            \
      << "[error] "

#endif  // !CC_LOGGING_LOGGER_HH
