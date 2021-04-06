/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#ifndef COMPRESSION_STREAM_MEMORY_STREAM_HH
#define COMPRESSION_STREAM_MEMORY_STREAM_HH

#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/stream.hh"

using namespace com::centreon::broker;

class CompressionStreamMemoryStream : public com::centreon::broker::io::stream {
 public:
  CompressionStreamMemoryStream()
      : io::stream("compression_stream_memory"),
        _shutdown(false),
        _timeout(false) {}

  std::shared_ptr<com::centreon::broker::io::raw>& get_buffer() {
    return _buffer;
  }

  bool read(std::shared_ptr<com::centreon::broker::io::data>& d,
            time_t deadline = (time_t)-1) {
    using namespace com::centreon::broker;
    (void)deadline;
    d.reset();
    if (_shutdown)
      throw exceptions::shutdown("{} is shutdown", __FUNCTION__);
    else if (_timeout)
      return false;
    d = _buffer;
    _buffer = std::shared_ptr<io::raw>();
    if (!d)
      throw exceptions::shutdown("{} has no more data", __FUNCTION__);
    return true;
  }

  int write(std::shared_ptr<com::centreon::broker::io::data> const& d) {
    using namespace com::centreon::broker;
    if (!d || d->type() != io::raw::static_type())
      throw com::centreon::exceptions::msg_fmt("invalid data sent to {}",
                                               __FUNCTION__);
    io::raw& e(*std::static_pointer_cast<io::raw>(d));
    if (!_buffer)
      _buffer.reset(new io::raw(e));
    else
      std::copy(e.get_buffer().begin(), e.get_buffer().end(),
                std::back_inserter(_buffer->get_buffer()));
    return 1;
  }

  int32_t stop() override { return 0; }

  void shutdown(bool shut_it_down = true) { _shutdown = shut_it_down; }

  void timeout(bool time_it_out = true) { _timeout = time_it_out; }

 private:
  std::shared_ptr<com::centreon::broker::io::raw> _buffer;
  bool _shutdown;
  bool _timeout;
};

#endif  // !COMPRESSION_STREAM_MEMORY_STREAM_HH
