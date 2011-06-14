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

#ifndef CCB_CONFIG_STATE_HH_
# define CCB_CONFIG_STATE_HH_

# include <QList>
# include <QMap>
# include <QString>
# include "config/endpoint.hh"
# include "config/logger.hh"

namespace                               com {
  namespace                             centreon {
    namespace                           broker {
      namespace                         config {
        /**
         *  @class state state.hh "config/state.hh"
         *  @brief Full configuration state.
         *
         *  A fully parsed configuration is represented within this
         *  class which holds mandatory parameters as well as optional
         *  parameters, along with object definitions.
         */
        class                           state {
         private:
          QList<endpoint>               _inputs;
          QList<logger>                 _loggers;
          QString                       _module_dir;
          QList<endpoint>               _outputs;
          QMap<QString, QString>        _params;
          void                          _internal_copy(state const& s);

         public:
                                        state();
                                        state(state const& s);
                                        ~state();
          state&                        operator=(state const& s);
          void                          clear();
          QList<endpoint>&              inputs();
          QList<endpoint> const&        inputs() const;
          QList<logger>&                loggers();
          QList<logger> const&          loggers() const;
          QString const&                module_directory() const;
          void                          module_directory(QString const& dir);
          QList<endpoint>&              outputs();
          QList<endpoint> const&        outputs() const;
          QMap<QString, QString>&       params();
          QMap<QString, QString> const& params() const;
        };
      }
    }
  }
}

#endif /* !CCB_CONFIG_STATE_HH_ */
