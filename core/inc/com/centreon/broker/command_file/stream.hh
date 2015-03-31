/*
** Copyright 2013 Merethis
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

#ifndef CCB_COMMAND_FILE_STREAM_HH
#  define CCB_COMMAND_FILE_STREAM_HH

#  include <string>
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/file/fifo.hh"

CCB_BEGIN()

namespace           command_file {
  /**
   *  @class stream stream.hh "com/centreon/broker/command_file/stream.hh"
   *  @brief Command file stream.
   *
   *  The class converts commands to NEB events.
   */
  class             stream : public io::stream {
  public:
                    stream(std::string const& filename);
                    ~stream();
    void            process(bool in = false, bool out = true);
    void            read(misc::shared_ptr<io::data>& d);
    void            statistics(io::properties& tree) const;
    unsigned int    write(misc::shared_ptr<io::data> const& d);

  private:
    bool            _process_in;
    bool            _process_out;

    std::string     _filename;
    file::fifo      _fifo;

                    stream(stream const& right);
    stream&         operator=(stream const& right);
  };
}

CCB_END()

#endif // !CCB_COMMAND_FILE_STREAM_HH
