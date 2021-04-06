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

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

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
  temporary_stream(std::string const& id = "")
      : io::stream("temporary_stream"), _id(id) {}
  /**
   *  Destructor.
   */
  ~temporary_stream() noexcept {}
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
      throw exceptions::msg_fmt(
          "temporary stream does not have any more event");
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
  int32_t write(std::shared_ptr<io::data> const& d) override {
    std::lock_guard<std::mutex> lock(_events_m);
    _events.push(d);
    return 1;
  }

  int32_t stop() override { return 0; }
};

CCB_END()

#endif  // !CCB_TEMPORARY_STREAM_HH
