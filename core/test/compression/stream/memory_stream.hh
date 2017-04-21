/*
** Copyright 2017 Centreon
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

#ifndef COMPRESSION_STREAM_MEMORY_STREAM_HH
#  define COMPRESSION_STREAM_MEMORY_STREAM_HH

#  include "com/centreon/broker/exceptions/shutdown.hh"
#  include "com/centreon/broker/io/stream.hh"

class  CompressionStreamMemoryStream : public com::centreon::broker::io::stream {
 public:
        CompressionStreamMemoryStream()
    : _shutdown(false), _timeout(false) {}

  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::raw>&
       get_buffer() {
    return (_buffer);
  }

  bool read(
         com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data>& d,
         time_t deadline = (time_t)-1) {
    using namespace com::centreon::broker;
    (void)deadline;
    d.clear();
    if (_shutdown)
      throw (exceptions::shutdown() << __FUNCTION__
             << " is shutdown");
    else if (_timeout)
      return (false);
    d = _buffer;
    _buffer = misc::shared_ptr<io::raw>();
    return (true);
  }

  int  write(
         com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> const& d) {
    using namespace com::centreon::broker;
    if (d.isNull() || (d->type() != io::raw::static_type()))
      throw (exceptions::msg()
             << "invalid data sent to " << __FUNCTION__);
    io::raw const& e(d.ref_as<io::raw>());
    if (_buffer.isNull())
      _buffer = new io::raw(e);
    else
      _buffer->append(e);
    return (1);
  }

  void shutdown(bool shut_it_down = true) {
    _shutdown = shut_it_down;
    return ;
  }

  void timeout(bool time_it_out = true) {
    _timeout = time_it_out;
    return ;
  }

 private:
  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::raw>
       _buffer;
  bool _shutdown;
  bool _timeout;
};

#endif // !COMPRESSION_STREAM_MEMORY_STREAM_HH
