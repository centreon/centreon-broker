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

#ifndef CCB_FILE_STREAM_HH_
# define CCB_FILE_STREAM_HH_

# include <QFile>
# include <QMutex>
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                    file {
  /**
   *  @class stream stream.hh "com/centreon/broker/file/stream.hh"
   *  @brief File stream.
   *
   *  Read and write data to a stream.
   */
  class                      stream : public io::stream {
   private:
    qint64                   _coffset;
    QFile                    _file;
    QMutex                   _mutex;
    bool                     _process_in;
    bool                     _process_out;
    qint64                   _roffset;
    qint64                   _woffset;
                             stream(stream const& s);
    stream&                  operator=(stream const& s);

   public:
                             stream(
                               QString const& filename,
                               QIODevice::OpenMode mode);
                             ~stream();
    void                     process(bool in = false, bool out = false);
    QSharedPointer<io::data> read();
    void                     write(QSharedPointer<io::data> d);
  };
}

CCB_END()

#endif /* !CCB_FILE_STREAM_HH_ */
