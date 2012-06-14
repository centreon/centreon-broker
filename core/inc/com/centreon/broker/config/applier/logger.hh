/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_CONFIG_APPLIER_LOGGER_HH
#  define CCB_CONFIG_APPLIER_LOGGER_HH

#  include <QList>
#  include <QMap>
#  include "com/centreon/broker/config/logger.hh"
#  include "com/centreon/broker/logging/backend.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            config {
  namespace          applier {
    /**
     *  @class logger logger.hh "com/centreon/broker/config/applier/logger.hh"
     *  @brief Open and close loggers.
     *
     *  According to some configuration, open and close logging objects
     *  as requested.
     */
    class            logger {
    public:
                     ~logger();
      void           apply(QList<config::logger> const& loggers);
      static logger& instance();

    private:
                     logger();
                     logger(logger const& l);
      logger&        operator=(logger const& l);
      void           _internal_copy(logger const& l);
      misc::shared_ptr<logging::backend>
                     _new_backend(config::logger const& cfg);

      QMap<config::logger, misc::shared_ptr<logging::backend> >
                     _backends;
    };
  }
}

CCB_END()

#endif // !CCB_CONFIG_APPLIER_LOGGER_HH
