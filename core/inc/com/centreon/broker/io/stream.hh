/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_IO_STREAM_HH
#  define CCB_IO_STREAM_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/properties.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                    io {
  /**
   *  @class stream stream.hh "com/centreon/broker/io/stream.hh"
   *  @brief Class used to exchange data.
   *
   *  Interface to exchange data.
   *
   *  The read() method is used to get data while waiting for it. It
   *  should work as follow :
   *    - has data :        shared pointer is properly set
   *    - spurious wake :   null shared pointer
   *    - known shutdown :  throw io::exceptions::shutdown
   *    - error :           any exception
   */
  class                      stream {
  public:
                             stream();
                             stream(stream const& s);
    virtual                  ~stream();
    stream&                  operator=(stream const& s);
    virtual void             process(
                               bool in = false,
                               bool out = true) = 0;
    virtual void             read(misc::shared_ptr<io::data>& d) = 0;
    virtual void             read(
                               misc::shared_ptr<io::data>& d,
                               time_t timeout,
                               bool* timed_out = NULL);
    virtual void             read_from(misc::shared_ptr<stream> from);
    virtual void             statistics(io::properties& tree) const;
    virtual void             update();
    virtual unsigned int     write(misc::shared_ptr<data> const& d) = 0;
    virtual void             write_to(misc::shared_ptr<stream> to);

  protected:
    misc::shared_ptr<stream> _from;
    misc::shared_ptr<stream> _to;
  };
}

CCB_END()

#endif // !CCB_IO_STREAM_HH
