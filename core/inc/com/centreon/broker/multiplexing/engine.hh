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

#ifndef CCB_MULTIPLEXING_ENGINE_HH
#  define CCB_MULTIPLEXING_ENGINE_HH

#  include <memory>
#  include <QObject>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/multiplexing/hooker.hh"

namespace                 com {
  namespace               centreon {
    namespace             broker {
      namespace           multiplexing {
        /**
         *  @class engine engine.hh "com/centreon/broker/multiplexing/engine.hh"
         *  @brief Multiplexing engine.
         *
         *  Send events from publishers to subscribers.
         *
         *  @see publisher
         *  @see subscriber
         */
        class             engine : public QObject {
          Q_OBJECT

         public:
                          ~engine();
          void            hook(hooker& h, bool data = true);
          static engine&  instance();
          static void     load();
          void            publish(misc::shared_ptr<io::data> const& d);
          void            start();
          void            stop();
          void            unhook(hooker& h);
          static void     unload();

         private:
                          engine();
                          engine(engine const& e);
          engine&         operator=(engine const& e);
          void            _nop(misc::shared_ptr<io::data> const& d);
          void            _send_to_subscribers();
          void            _write(misc::shared_ptr<io::data> const& d);

          static std::auto_ptr<engine>
                          _instance;
          void (engine::* _write_func)(
                            misc::shared_ptr<io::data> const&);

         private slots:
          void            _on_hook_destroy(QObject* obj);
        };
      }
    }
  }
}

#endif // !CCB_MULTIPLEXING_ENGINE_HH
