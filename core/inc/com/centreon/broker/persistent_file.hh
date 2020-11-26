/*
** Copyright 2015,2017 Centreon
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

#ifndef CCB_PERSISTENT_FILE_HH
#define CCB_PERSISTENT_FILE_HH

#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class persistent_file persistent_file.hh
 * "com/centreon/broker/persistent_file.hh"
 *  @brief On-disk file.
 *
 *  On-disk file that uses multiple streams to write serialized data.
 *  It uses BBDO, compression and file streams.
 */
class persistent_file : public io::stream {
 public:
  persistent_file(std::string const& path);
  ~persistent_file() noexcept;
  bool read(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  void remove_all_files();
  void statistics(json11::Json::object& tree) const;
  int write(std::shared_ptr<io::data> const& d);

 private:
  persistent_file(persistent_file const& other);
  persistent_file& operator=(persistent_file const& other);

  std::shared_ptr<file::stream> _splitter;
};

CCB_END()

#endif  // !CCB_PERSISTENT_FILE_HH
