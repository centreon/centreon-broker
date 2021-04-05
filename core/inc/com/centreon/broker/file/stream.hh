/*
** Copyright 2011-2015,2017, 2020-2021 Centreon
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

#ifndef CCB_FILE_STREAM_HH
#define CCB_FILE_STREAM_HH

#include <memory>
#include <mutex>
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class stream stream.hh "com/centreon/broker/file/stream.hh"
 *  @brief File stream.
 *
 *  Read and write data to a stream.
 */
class stream : public io::stream {
  std::unique_ptr<splitter> _file;
  mutable long long _last_read_offset;
  mutable time_t _last_time;
  mutable long long _last_write_offset;

 public:
  stream(splitter* file);
  ~stream();
  stream(const stream&) = delete;
  stream& operator=(const stream&) = delete;
  std::string peer() const;
  bool read(std::shared_ptr<io::data>& d, time_t deadline) override;
  void remove_all_files();
  void statistics(json11::Json::object& tree) const override;
  int32_t write(std::shared_ptr<io::data> const& d) override;
  int32_t stop() override;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_STREAM_HH
