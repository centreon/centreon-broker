/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_PROCESSING_FEEDER_HH
#  define CCB_PROCESSING_FEEDER_HH

#  include <QThread>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         processing {
        /**
         *  @class feeder feeder.hh "com/centreon/broker/processing/feeder.hh"
         *  @brief Feed events from a source to a destination.
         *
         *  Take events from a source and send them to a destination.
         */
        class           feeder : public QThread {
          Q_OBJECT

         public:
                        feeder();
                        feeder(feeder const& f);
                        ~feeder();
          feeder&       operator=(feeder const& f);
          void          exit();
          void          prepare(
                          misc::shared_ptr<io::stream> in,
                          misc::shared_ptr<io::stream> out);
          void          run();

         private:
          misc::shared_ptr<io::stream>
                        _in;
          misc::shared_ptr<io::stream>
                        _out;
          volatile bool _should_exit;
        };
      }
    }
  }
}

#endif // !CCB_PROCESSING_FEEDER_HH
