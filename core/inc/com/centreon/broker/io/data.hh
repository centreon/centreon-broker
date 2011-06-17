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

#ifndef CCB_IO_DATA_HH_
# define CCB_IO_DATA_HH_

# include <QString>

namespace                        com {
  namespace                      centreon {
    namespace                    broker {
      namespace                  io {
        /**
         *  @class data data.hh "com/centreon/broker/io/data.hh"
         *  @brief Data abstraction.
         *
         *  Data is the core element that is transmitted through
         *  Centreon Broker.
         */
        class                    data {
         public:
                                 data();
                                 data(data const& d);
          virtual                ~data();
          data&                  operator=(data const& d);
          virtual QString const& type() const = 0;
        };
      }
    }
  }
}

#endif /* !CCB_IO_DATA_HH_ */
