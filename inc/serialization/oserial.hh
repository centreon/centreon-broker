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

#ifndef CCB_SERIALIZATION_OSERIAL_HH_
# define CCB_SERIALIZATION_OSERIAL_HH_

# include <QSharedPointer>
# include "events/event.hh"
# include "io/ostream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        serialization {
        /**
         *  @class oserial oserial.hh "serialization/oserial.hh"
         *  @brief Convert events to binary data.
         *
         *  Interface for protocol layers which convert events to binary
         *  data.
         */
        class          oserial : virtual public com::centreon::broker::io::ostream {
         public:
                       oserial();
                       oserial(oserial const& o);
          virtual      ~oserial();
          oserial&     operator=(oserial const& o);
          virtual void serialize(QSharedPointer<com::centreon::broker::events::event> e) = 0;
        };
      }
    }
  }
}

#endif /* !CCB_SERIALIZATION_OSERIAL_HH_ */
