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

#ifndef CCB_COMPRESSION_STREAM_HH_
# define CCB_COMPRESSION_STREAM_HH_

# include "com/centreon/broker/io/stream.hh"

namespace                          com {
  namespace                        centreon {
    namespace                      broker {
      namespace                    compression {
        /**
         *  @class stream stream.hh "com/centreon/broker/compression/stream.hh"
         *  @brief Compression stream.
         *
         *  Compress and uncompress data.
         */
        class                      stream : public io::stream {
         private:
          int                      _level;
          QByteArray               _rbuffer;
          unsigned int             _size;
          QByteArray               _wbuffer;
          void                     _flush();
          bool                     _get_data(unsigned int size);
          void                     _internal_copy(stream const& s);

         public:
                                   stream(int level = -1,
                                     unsigned int size = 0);
                                   stream(stream const& s);
                                   ~stream();
          stream&                  operator=(stream const& s);
          QSharedPointer<io::data> read();
          void                     write(QSharedPointer<io::data> d);
        };
      }
    }
  }
}

#endif /* !CCB_COMPRESSION_STREAM_HH_ */
