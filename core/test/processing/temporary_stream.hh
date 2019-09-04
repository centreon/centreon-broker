/*
** Copyright 2011-2013,2015 Centreon
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

#ifndef CCB_TEMPORARY_STREAM_HH
#define CCB_TEMPORARY_STREAM_HH

#include <mutex>
#include <queue>
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class temporary_stream temporary_stream.hh
 *  @brief Temporary stream.
 */
class temporary_stream : public io::stream {
 public:
  temporary_stream(std::string const& id = "");
  temporary_stream(temporary_stream const& ss);
  ~temporary_stream();
  temporary_stream& operator=(temporary_stream const& ss);
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  int write(std::shared_ptr<io::data> const& d);

 private:
  std::queue<std::shared_ptr<io::data>> _events;
  mutable std::mutex _eventsm;
  std::string _id;
};

CCB_END()

#endif  // !CCB_TEMPORARY_STREAM_HH
