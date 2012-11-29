/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_FILE_STREAM_HH
#  define CCB_FILE_STREAM_HH

#  include <QMutex>
#  include "com/centreon/broker/file/cfile.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        file {
  /**
   *  @class stream stream.hh "com/centreon/broker/file/stream.hh"
   *  @brief File stream.
   *
   *  Read and write data to a stream.
   */
  class          stream : public io::stream {
  public:
                 stream(
                   std::string const& path,
                   unsigned long long max_size = (unsigned long long)-1);
                 ~stream();
    void         process(
                   bool in = false,
                   bool out = false);
    void         read(misc::shared_ptr<io::data>& d);
    void         statistics(std::string& buffer) const;
    void         write(misc::shared_ptr<io::data> const& d);

  private:
                 stream(stream const& s);
    stream&      operator=(stream const& s);
    std::string  _file_path(unsigned int id) const;
    void         _open_first_read();
    void         _open_first_write();
    void         _open_next_read();
    void         _open_next_write(bool truncate = true);

    long         _max_size;
    QMutex       _mutex;
    std::string  _path;
    bool         _process_in;
    bool         _process_out;
    misc::shared_ptr<cfile>
                 _rfile;
    unsigned int _rid;
    long         _roffset;
    misc::shared_ptr<cfile>
                 _wfile;
    unsigned int _wid;
    long         _woffset;
  };
}

CCB_END()

#endif // !CCB_FILE_STREAM_HH
