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

#ifndef CCB_SERIALIZATION_ISERIAL_HH_
# define CCB_SERIALIZATION_ISERIAL_HH_

# include <QSharedPointer>
# include "events/event.hh"
# include "io/istream.hh"

namespace          com {
  namespace        centreon {
    namespace      broker {
      namespace    serialization {
        /**
         *  @class iserial iserial.hh "serialization/iserial.hh"
         *  @brief Convert binary data stream to events.
         *
         *  Interface for protocol layers which convert binary data to
         *  event structures.
         */
        class      iserial : virtual public com::centreon::broker::io::istream {
         public:
                   iserial();
                   iserial(iserial const& i);
          virtual  ~iserial();
          iserial& operator=(iserial const& i);
          virtual QSharedPointer<com::centreon::broker::events::event>
                   deserialize() = 0;
        };
      }
    }
  }
}

#endif /* !CCB_SERIALIZATION_ISERIAL_HH_ */
