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

#ifndef CCB_PROCESSING_FEEDER_HH_
# define CCB_PROCESSING_FEEDER_HH_

# include <QSharedPointer>
# include <QThread>
# include "io/stream.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         processing {
        /**
         *  @class feeder feeder.hh "processing/feeder.hh"
         *  @brief Feed events from a source to a destination.
         *
         *  Take events from a source and send them to a destination.
         */
        class           feeder : public QThread {
          Q_OBJECT

         private:
          QSharedPointer<com::centreon::broker::io::stream>
                        _in;
          QSharedPointer<com::centreon::broker::io::stream>
                        _out;
          volatile bool _should_exit;

         public:
                        feeder();
                        feeder(feeder const& f);
                        ~feeder();
          feeder&       operator=(feeder const& f);
          void          exit();
          void          prepare(QSharedPointer<com::centreon::broker::io::stream> in,
                          QSharedPointer<com::centreon::broker::io::stream> out);
          void          run();
        };
      }
    }
  }
}

#endif /* !CCB_PROCESSING_FEEDER_HH_ */
