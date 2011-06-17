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

#ifndef CCB_IO_RAW_HH_
# define CCB_IO_RAW_HH_

# include <QByteArray>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          io {
        /**
         *  @class raw raw.hh "io/raw.hh"
         *  @brief Raw byte array.
         *
         *  Raw byte array.
         */
        class            raw : public data, public QByteArray {
         public:
                         raw();
                         raw(raw const& r);
                         ~raw();
          raw&           operator=(raw const& r);
          void*          memory();
          void const*    memory() const;
          unsigned int   size() const;
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_IO_RAW_HH_ */
