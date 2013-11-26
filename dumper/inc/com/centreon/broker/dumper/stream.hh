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

#ifndef CCB_DUMPER_STREAM_HH
#  define CCB_DUMPER_STREAM_HH

#  include <QMutex>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              dumper {
  /**
   *  @class stream stream.hh "com/centreon/broker/dumper/stream.hh"
   *  @brief Dumper stream.
   *
   *  Read and write data to a stream.
   */
  class                stream : public io::stream {
  public:
                       stream(
                         std::string const& path,
                         std::string const& tagname);
                       ~stream();
    void               process(
                         bool in = false,
                         bool out = false);
    void               read(misc::shared_ptr<io::data>& d);
    unsigned int       write(misc::shared_ptr<io::data> const& d);

  private:
                       stream(stream const& s);
    stream&            operator=(stream const& s);

    QMutex             _mutex;
    std::string        _path;
    bool               _process_in;
    bool               _process_out;
    std::string        _tagname;
  };
}

CCB_END()

#endif // !CCB_DUMPER_STREAM_HH
