/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_LOGGING_LOGGER_HH
#  define CC_LOGGING_LOGGER_HH

#  include "com/centreon/logging/engine.hh"
#  include "com/centreon/logging/temp_logger.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace logging {
  enum    type_value {
    type_info = (1ull << 61),
    type_debug = (1ull << 62),
    type_error = (1ull << 63)
  };

  enum    verbosity_level {
    low = 0,
    medium = 1,
    high = 2
  };
}

CC_END()

#  define log_info(verbose) \
  for (unsigned int __com_centreon_logging_define_ui(0); \
       !__com_centreon_logging_define_ui \
       && com::centreon::logging::engine::instance().is_log( \
               com::centreon::logging::type_info, \
               verbose); \
       ++__com_centreon_logging_define_ui) \
    com::centreon::logging::temp_logger( \
      com::centreon::logging::type_info, \
      verbose) \
        << "[info] "

#  define log_debug(verbose) \
  for (unsigned int __com_centreon_logging_define_ui(0); \
       !__com_centreon_logging_define_ui \
       && com::centreon::logging::engine::instance().is_log(   \
               com::centreon::logging::type_debug, \
               verbose); \
       ++__com_centreon_logging_define_ui) \
    com::centreon::logging::temp_logger( \
      com::centreon::logging::type_debug, \
      verbose) \
      << "[debug] "

#  define log_error(verbose) \
  for (unsigned int __com_centreon_logging_define_ui(0); \
       !__com_centreon_logging_define_ui \
       && com::centreon::logging::engine::instance().is_log( \
               com::centreon::logging::type_error, \
               verbose); \
       ++__com_centreon_logging_define_ui) \
    com::centreon::logging::temp_logger( \
      com::centreon::logging::type_error, \
      verbose) \
      << "[error] "

#endif // !CC_LOGGING_LOGGER_HH
