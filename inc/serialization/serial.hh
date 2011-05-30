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

#ifndef CCB_SERIALIZATION_SERIAL_HH_
# define CCB_SERIALIZATION_SERIAL_HH_

# include "serialization/iserial.hh"
# include "serialization/oserial.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   serialization {
        /**
         *  @class serial serial.hh "serialization/serial.hh"
         *  @brief Both way serialization.
         *
         *  Serialize and deserialize events.
         */
        class     serial : virtual public iserial,
                           virtual public oserial {
         public:
                  serial();
                  serial(serial const& s);
          virtual ~serial();
          serial& operator=(serial const& s);
        };
      }
    }
  }
}

#endif /* !CCB_SERIALIZATION_SERIAL_HH_ */
