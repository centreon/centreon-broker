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

#ifndef CCB_TEMPORARY_STREAM_HH
#define CCB_TEMPORARY_STREAM_HH

#include <mutex>
#include <queue>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class temporary_stream temporary_stream.hh
 *  @brief Temporary stream.
 */
class temporary_stream : public io::stream {
  std::queue<std::shared_ptr<io::data>> _events;
  mutable std::mutex _events_m;
  std::string _id;

 public:
  /**
   *  Constructor.
   *
   *  @param[in] id The temporary id.
   */
  temporary_stream(std::string const& id = "") : _id(id) {}
  /**
   *  Copy constructor.
   *
   *  @param[in] ss Object to copy.
   */
  temporary_stream(temporary_stream const& ss)
      : io::stream(ss), _events(ss._events), _id(ss._id) {}
  /**
   *  Destructor.
   */
  ~temporary_stream() noexcept {}
  /**
   *  Assignment operator.
   *
   *  @param[in] ss Object to copy.
   *
   *  @return This object.
   */
  temporary_stream& operator=(temporary_stream const& ss) {
    if (this != &ss) {
      io::stream::operator=(ss);
      std::lock_guard<std::mutex> lock1(_events_m);
      std::lock_guard<std::mutex> lock2(ss._events_m);
      _events = ss._events;
      _id = ss._id;
    }
    return *this;
  }
  /**
   *  Read some data.
   *
   *  @param[out] data      Some data.
   *  @param[in]  deadline  Unused.
   *
   *  @return Always return true.
   */
  bool read(std::shared_ptr<io::data>& d, time_t deadline) {
    (void)deadline;
    std::lock_guard<std::mutex> lock(_events_m);
    if (_events.empty())
      throw exceptions::msg()
          << "temporary stream does not have any more event";
    else
      d = _events.front();
    _events.pop();
    return true;
  }

  /**
   *  Write some data.
   *
   *  @param[in] d Data to write.
   *
   *  @return Number of elements acknowledged (1).
   */
  int write(std::shared_ptr<io::data> const& d) {
    std::lock_guard<std::mutex> lock(_events_m);
    _events.push(d);
    return 1;
  }
};

CCB_END()

#endif  // !CCB_TEMPORARY_STREAM_HH
