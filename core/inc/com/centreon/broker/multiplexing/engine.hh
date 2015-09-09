/*
** Copyright 2009-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_MULTIPLEXING_ENGINE_HH
#  define CCB_MULTIPLEXING_ENGINE_HH

#  include <memory>
#  include <QMutex>
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
        class             engine : public QObject, public QMutex {
          Q_OBJECT

         public:
                          ~engine();
          void            clear();
          void            hook(hooker& h, bool data = true);
          static engine&  instance();
          static void     load();
          void            publish(misc::shared_ptr<io::data> const& d);
          void            start();
          void            stop();
          bool            stopped() const throw ();
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
          bool            _stopped;
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
