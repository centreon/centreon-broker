/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_LOGGING_MANAGER_HH_
# define CCB_LOGGING_MANAGER_HH_

# include <memory>
# include <QObject>
# include <QReadWriteLock>
# include <QVector>
# include "com/centreon/broker/logging/backend.hh"
# include "com/centreon/broker/logging/defines.hh"
# include "com/centreon/broker/logging/temp_logger.hh"

namespace                 com {
  namespace               centreon {
    namespace             broker {
      namespace           logging {
        /**
         *  @class manager manager.hh "com/centreon/broker/logging/manager.hh"
         *  @brief Logging object manager.
         *
         *  Register logging objects. This is the external access point
         *  to the logging system.
         */
        class             manager : public backend {
          Q_OBJECT

         private:
          struct          manager_backend {
            backend*      b;
            level         l;
            unsigned int  types;
          };
          QVector<manager_backend>
                          _backends;
          QReadWriteLock  _backendsm;
          static std::auto_ptr<manager>
                          _instance;
          unsigned int    _limits[4];
                          manager();
                          manager(manager const& m);
          manager&        operator=(manager const& m);
          void            _compute_optimizations();

         private slots:
          void            _on_backend_destruction(QObject* obj);

         public:
                          ~manager();
          temp_logger     get_temp_logger(type t, level l) throw ();
          static manager& instance();
          static void     load();
          void            log_msg(
                            char const* msg,
                            unsigned int len,
                            type t,
                            level l) throw ();
          void            log_on(
                            backend& b,
                            unsigned int
                              types = config_type
                                      | debug_type
                                      | error_type
                                      | info_type,
                            level min_priority = medium);
          static void     unload();
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_MANAGER_HH_ */
