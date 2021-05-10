/*
** Copyright 2011-2013,2015,2017 Centreon
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
#define CCB_IO_STREAM_HH

#include <ctime>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class stream stream.hh "com/centreon/broker/io/stream.hh"
 *  @brief Class used to exchange data.
 *
 *  Interface to exchange data.
 *
 *  The read() method is used to get data while waiting for it. It
 *  should work as follow:
 *    - has data:        shared pointer is properly set, true returned
 *    - spurious wake:   null shared pointer, true returned
 *    - timeout:         null shared pointer, false returned
 *    - known shutdown:  throw io::exceptions::shutdown
 *    - error:           any exception
 *
 *  The write() method is used to send data through the stream. It
 *  should return the number of event fully written through (taking into
 *  account any buffering, or underlayer) to the end device. If that
 *  information is not available or meaningful, it should always return '1'.
 *
 *  Behind a stream, we can have threads doing complicated things. Before
 *  destroying a stream, we have to stop all these threads correctly, to flush
 *  pending events, all these things are the purpose of the stop() internal
 *  function.
 */
class stream {
  const std::string _name;

 protected:
  std::shared_ptr<stream> _substream;

 public:
  stream(const std::string& name);
  virtual ~stream() noexcept = default;
  virtual int32_t stop() = 0;
  stream(const stream&) = delete;
  stream& operator=(const stream&) = delete;
  virtual int flush();
  virtual std::string peer() const;
  virtual bool read(std::shared_ptr<io::data>& d,
                    time_t deadline = (time_t)-1) = 0;
  virtual void set_substream(std::shared_ptr<stream> substream);
  std::shared_ptr<stream> get_substream();
  virtual void statistics(nlohmann::json& tree) const;
  virtual void update();
  bool validate(std::shared_ptr<io::data> const& d, std::string const& error);
  virtual int write(std::shared_ptr<data> const& d) = 0;
  const std::string& get_name() const { return _name; }
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_STREAM_HH
