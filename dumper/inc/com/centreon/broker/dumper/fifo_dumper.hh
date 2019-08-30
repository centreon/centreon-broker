/*
** Copyright 2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_DUMPER_FIFO_DUMPER_HH
#  define CCB_DUMPER_FIFO_DUMPER_HH

#  include <fstream>
#  include <memory>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/file/fifo.hh"

CCB_BEGIN()

namespace              dumper {
  /**
   *  @class fifo_dumper fifo_dumper.hh "com/centreon/broker/dumper/fifo_dumper.hh"
   *  @brief Fifo dumper.
   *
   *  Read from a fifo and dump it.
   */
  class                fifo_dumper : public io::stream {
  public:
                       fifo_dumper(
                         std::string const& path,
                         std::string const& tagname);
                       ~fifo_dumper();
    bool               read(
                         std::shared_ptr<io::data>& d,
                         time_t deadline);
    int                write(std::shared_ptr<io::data> const& d);

  private:
                       fifo_dumper(fifo_dumper const& s);
    fifo_dumper&       operator=(fifo_dumper const& s);

    std::string        _path;
    std::string        _tagname;
    file::fifo         _fifo;
  };
}

CCB_END()

#endif // !CCB_DUMPER_FIFO_DUMPER_HH
