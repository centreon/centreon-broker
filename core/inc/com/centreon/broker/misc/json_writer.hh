/*
** Copyright 2015 Centreon
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

#ifndef CCB_MISC_JSON_WRITER_HH
#define CCB_MISC_JSON_WRITER_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
/**
 *  @class json_writer json_writer.hh "com/centreon/json/json_writer.hh"
 *  @brief Simple json writer.
 */
class json_writer {
 public:
  json_writer();
  ~json_writer();

  std::string const& get_string() const;

  void open_object();
  void close_object();
  void open_array();
  void close_array();
  void add_key(std::string const& key);
  void add_string(std::string const& str);
  void add_number(long long number);
  void add_double(double number);
  void add_null();
  void add_boolean(bool value);
  void merge(std::string const& val);

 private:
  std::string _string;

  void _put_comma();

  json_writer(json_writer const&);
  json_writer& operator=(json_writer const&);
};
}  // namespace misc

CCB_END()

#endif  // !CCB_MISC_JSON_WRITER_HH
