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

#ifndef CCB_IO_STREAM_HH_
# define CCB_IO_STREAM_HH_

# include <QSharedPointer>

namespace                        com {
  namespace                      centreon {
    namespace                    broker {
      namespace                  io {
        /**
         *  @class stream stream.hh "io/stream.hh"
         *  @brief Class used to exchange data.
         *
         *  Interface to exchange data.
         */
        class                    stream {
         protected:
          QSharedPointer<stream> _from;
	  QSharedPointer<stream> _to;

         public:
                                 stream();
                                 stream(stream const& s);
          virtual                ~stream();
          stream&                operator=(stream const& s);
          virtual unsigned int   read(void* data, unsigned int size) = 0;
          void                   read_from(QSharedPointer<stream> from);
          virtual unsigned int   write(void* data, unsigned int size) = 0;
          void                   write_to(QSharedPointer<stream> to);
        };
      }
    }
  }
}

#endif /* !CCB_IO_STREAM_HH_ */
