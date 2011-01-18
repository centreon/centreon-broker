/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef PROCESSING_LISTENER_HH_
# define PROCESSING_LISTENER_HH_

# include <memory>
# include "concurrency/mutex.hh"
# include "concurrency/thread.hh"
# include "io/acceptor.hh"

namespace                       processing {
  /**
   *  @class listener listener.hh "processing/listener.hh"
   *  @brief Waits for incoming connections on an acceptor.
   *
   *  The listener class implements a mediator pattern and avoid
   *  acceptors from being tightly coupled with other objects. listener
   *  can either wait on input acceptors or output acceptors.
   */
  class                         listener : public concurrency::thread {
   public:
    enum                        INOUT {
      UNKNOWNIO = 0,
      IN,
      OUT
    };
    enum                        protocol {
      UNKNOWNPROTO = 0,
      NDO,
      XML
    };

   private:
    std::auto_ptr<io::acceptor> _acceptor;
    bool                        _init;
    concurrency::mutex          _initm;
    void                        (listener::* _run_thread)(io::stream*);
                                listener(listener const& l);
    listener&                   operator=(listener const& l);
    void                        run_ndo_in(io::stream* s);
    void                        run_ndo_out(io::stream* s);
    //    void                        run_xml_in(io::stream* s);
    void                        run_xml_out(io::stream* s);

   public:
                                listener();
                                ~listener();
    void                        operator()();
    void                        exit();
    void                        init(io::acceptor* a,
                                  protocol proto,
                                  INOUT io,
                                  concurrency::thread_listener* tl = NULL);
  };
}

#endif /* !PROCESSING_LISTENER_HH_ */
