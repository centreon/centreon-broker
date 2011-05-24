/*
** Copyright 2009-2011 MERETHIS
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

#ifndef CCB_LOGGING_DEFINES_HH_
# define CCB_LOGGING_DEFINES_HH_

namespace com {
  namespace centreon {
    namespace broker {
      namespace logging {
        // Log levels.
        enum    level {
          NONE = 0,
          HIGH,
          MEDIUM,
          LOW
        };

        // Log types.
        enum    type {
          CONFIG = 1,
          DEBUG = 2,
          ERROR = 4,
          INFO = 8
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_DEFINES_HH_ */
