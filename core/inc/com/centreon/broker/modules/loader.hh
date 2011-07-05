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

#ifndef CCB_MODULES_LOADER_HH_
# define CCB_MODULES_LOADER_HH_

# include <QHash>
# include <QSharedPointer>
# include <QString>
# include "com/centreon/broker/modules/handle.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   modules {
        /**
         *  @class loader loader.hh "com/centreon/broker/modules/loader.hh"
         *  @brief Load Centreon Broker plugins.
         *
         *  This class is used to load Centreon Broker plugins and hold
         *  reference to them as long as they exist.
         */
        class     loader {
         private:
          QHash<QString, QSharedPointer<handle> >
                  _handles;

         public:
                  loader();
                  loader(loader const& l);
                  ~loader();
          loader& operator=(loader const& l);
          void    load_dir(QString const& dirname,
                    void const* arg = NULL);
          void    load_file(QString const& filename,
                    void const* arg = NULL);
        };
      }
    }
  }
}

#endif /* !CCB_MODULES_LOADER_HH_ */
