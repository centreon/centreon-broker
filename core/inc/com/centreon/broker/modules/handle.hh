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

#ifndef CCB_MODULES_HANDLE_HH_
# define CCB_MODULES_HANDLE_HH_

# include <QLibrary>
# include <QSharedPointer>

namespace                    com {
  namespace                  centreon {
    namespace                broker {
      namespace              modules {
        /**
         *  @class handle handle.hh "com/centreon/broker/modules/handle.hh"
         *  @brief Plugin library handle.
         *
         *  Centreon Broker can load plugins. This class represents such
         *  plugins.
         */
        class                handle {
         public:
          static char const* deinitialization;
          static char const* initialization;

         private:
          QLibrary           _handle;
          void               _init();

         public:
                             handle();
                             handle(handle const& h);
                             ~handle();
          handle&            operator=(handle const& h);
          void               close();
          bool               is_open() const;
          void               open(QString const& filename);
        };
      }
    }
  }
}

#endif /* !CCB_MODULES_HANDLE_HH_ */
