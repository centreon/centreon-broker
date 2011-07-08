/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_LOGGING_LOGGING_HH_
# define CCB_LOGGING_LOGGING_HH_

# include <QSharedPointer>
# include "com/centreon/broker/logging/backend.hh"
# include "com/centreon/broker/logging/logger.hh"
# include "com/centreon/broker/logging/void_logger.hh"

namespace                  com {
  namespace                centreon {
    namespace              broker {
      namespace            logging {
        // Standard logging objects.
        extern logger      config;
# ifdef NDEBUG
        extern void_logger debug;
# else
        extern logger      debug;
# endif /* NDEBUG */
        extern logger      error;
        extern logger      info;

        void               clear();
        void               log_on(QSharedPointer<backend> b,
                             unsigned int types = CONFIG | DEBUG | ERROR | INFO,
                             level min_priority = HIGH);
      }
    }
  }
}

#endif /* !CCB_LOGGING_LOGGING_HH_ */
