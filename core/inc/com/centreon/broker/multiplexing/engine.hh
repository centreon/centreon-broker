/*
** Copyright 2009-2012,2015 Merethis
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

#  include <QMutex>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/multiplexing/hooker.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           multiplexing {
  // Forward declaration.
  class             muxer;

  /**
   *  @class engine engine.hh "com/centreon/broker/multiplexing/engine.hh"
   *  @brief Multiplexing engine.
   *
   *  Core multiplexing engine. Send events to and receive events from
   *  muxer objects.
   *
   *  @see muxer
   */
  class             engine : public QMutex {
  public:
                    ~engine();
    void            clear();
    void            hook(hooker& h, bool with_data = true);
    static engine&  instance();
    static void     load();
    void            publish(misc::shared_ptr<io::data> const& d);
    void            start();
    void            stop();
    void            subscribe(muxer* subscriber);
    void            unhook(hooker& h);
    static void     unload();
    void            unsubscribe(muxer* subscriber);

  private:
                    engine();
                    engine(engine const& other);
    engine&         operator=(engine const& other);
    void            _nop(misc::shared_ptr<io::data> const& d);
    void            _send_to_subscribers();
    void            _write(misc::shared_ptr<io::data> const& d);

    static engine*  _instance;
    void (engine::* _write_func)(
                      misc::shared_ptr<io::data> const&);
  };
}

CCB_END()

#endif // !CCB_MULTIPLEXING_ENGINE_HH
