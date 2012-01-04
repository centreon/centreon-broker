/*
** Copyright 2011 Merethis
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

#ifndef CCB_MULTIPLEXING_HOOKER_HH_
# define CCB_MULTIPLEXING_HOOKER_HH_

# include <QObject>
# include "com/centreon/broker/io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        multiplexing {
        /**
         *  @class hooker hooker.hh "com/centreon/broker/multiplexing/hooker.hh"
         *  @brief Hook object.
         *
         *  Place a hook on the multiplexing engine.
         */
        class          hooker : public QObject, public io::stream {
          Q_OBJECT

         protected:
          bool         _registered;

         public:
                       hooker();
                       hooker(hooker const& h);
          virtual      ~hooker();
          hooker&      operator=(hooker const &h);
          virtual void process(bool in = false, bool out = true);
          virtual void starting() = 0;
          virtual void stopping() = 0;
        };
      }
    }
  }
}

#endif /* !CCB_MULTIPLEXING_HOOKER_HH_ */
