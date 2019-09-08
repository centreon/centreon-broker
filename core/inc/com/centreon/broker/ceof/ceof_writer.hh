/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_CEOF_CEOF_WRITER_HH
#define CCB_CEOF_CEOF_WRITER_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_writer ceof_writer.hh "com/centreon/broker/ceof/ceof_writer.hh"
 *  @brief Centreon Engine Object File writer.
 */
class ceof_writer {
 public:
  ceof_writer();
  ceof_writer(ceof_writer const& other);
  ceof_writer& operator=(ceof_writer const& other);
  ~ceof_writer() throw();

  void open_object(std::string const& object_type);
  void add_key(std::string const& key);
  void add_value(std::string const& value);
  void close_object();

  std::string const& get_string() const throw();

 private:
  std::string _str;
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_WRITER_HH
