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

#ifndef CCB_IO_ISTREAM_HH_
# define CCB_IO_ISTREAM_HH_

# include <QSharedPointer>

namespace                         com {
  namespace                       centreon {
    namespace                     broker {
      namespace                   io {
        /**
         *  @class istream istream.hh "io/istream.hh"
         *  @brief Stream that can be read.
         *
         *  Stream from which data can be fetched.
         */
        class                     istream {
         protected:
          QSharedPointer<istream> _from;

         public:
                                  istream();
                                  istream(istream const& i);
          virtual                 ~istream();
          istream&                operator=(istream const& i);
          virtual unsigned int    read(void* data, unsigned int size) = 0;
          void                    read_from(QSharedPointer<istream> from);
        };
      }
    }
  }
}

#endif /* !CCB_IO_ISTREAM_HH_ */
