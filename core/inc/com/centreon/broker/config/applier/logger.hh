/*
** Copyright 2011 Merethis
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

#ifndef CCB_CONFIG_APPLIER_LOGGER_HH_
# define CCB_CONFIG_APPLIER_LOGGER_HH_

# include <QList>
# include <QMap>
# include <QSharedPointer>
# include "com/centreon/broker/config/logger.hh"
# include "com/centreon/broker/logging/backend.hh"

namespace                  com {
  namespace                centreon {
    namespace              broker {
      namespace            config {
        namespace          applier {
          /**
           *  @class logger logger.hh "com/centreon/broker/config/applier/logger.hh"
           *  @brief Open and close loggers.
           *
           *  According to some configuration, open and close logging
           *  objects as requested.
           */
          class            logger {
           private:
            QMap<config::logger, QSharedPointer<logging::backend> >
                           _backends;
                           logger();
                           logger(logger const& l);
            logger&        operator=(logger const& l);
            QSharedPointer<logging::backend>
                           _new_backend(config::logger const& cfg);

           public:
                           ~logger();
            void           apply(QList<config::logger> const& loggers);
            static logger& instance();
          };
        }
      }
    }
  }
}

#endif /* !CCB_CONFIG_APPLIER_LOGGER_HH_ */
