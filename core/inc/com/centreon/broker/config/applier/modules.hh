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

#ifndef CCB_CONFIG_APPLIER_MODULES_HH_
# define CCB_CONFIG_APPLIER_MODULES_HH_

# include <QString>
# include "com/centreon/broker/modules/loader.hh"

namespace                           com {
  namespace                         centreon {
    namespace                       broker {
      namespace                     config {
        namespace                   applier {
          /**
           *  @class modules modules.hh "com/centreon/broker/config/applier/modules.hh"
           *  @brief Load necessary modules.
           *
           *  Load modules as per the configuration.
           */
          class                     modules {
           private:
            broker::modules::loader _loader;
                                    modules();
                                    modules(modules const& m);
            modules&                operator=(modules const& m);

           public:
                                    ~modules();
            void                    apply(QString const& module_dir,
                                      void const* arg = NULL);
            static modules&         instance();
            void                    unload();
          };
        }
      }
    }
  }
}

#endif /* !CCB_CONFIG_APPLIER_MODULES_HH_ */
