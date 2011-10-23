/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_MULTIPLEXING_SUBSCRIBER_HH_
# define CCB_MULTIPLEXING_SUBSCRIBER_HH_

# include <QMutex>
# include <QQueue>
# include <QWaitCondition>
# include <time.h>
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          multiplexing {
  /**
   *  @class subscriber subscriber.hh "com/centreon/broker/multiplexing/subscriber.hh"
   *  @brief Receive events from publishers and make them
   *         available through the interface::source interface.
   *
   *  This class is used as a cornerstone in event multiplexing.
   *  Each output willing to receive events will request a
   *  subscriber object. All publisher objects broadcast events
   *  they receive to every subscriber objects.
   *
   *  @see publisher
   */
  class            subscriber : public io::stream {
   private:
    QWaitCondition _cv;
    QQueue<QSharedPointer<io::data> >
                   _events;
    QMutex         _mutex;
    bool           _registered;
                   subscriber(subscriber const& s);
    subscriber&    operator=(subscriber const& s);
    void           clean();

   public:
                   subscriber();
                   ~subscriber();
    void           process(bool in = false, bool out = true);
    QSharedPointer<io::data>
                   read();
    QSharedPointer<io::data>
                   read(time_t deadline);
    void           write(QSharedPointer<io::data> d);
  };
}

CCB_END()

#endif /* !CCB_MULTIPLEXING_SUBSCRIBER_HH_ */
