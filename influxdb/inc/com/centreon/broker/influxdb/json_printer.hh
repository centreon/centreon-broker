/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_INFLUXDB_INFLUXDB_JSON_PRINTER_HH
#define CCB_INFLUXDB_INFLUXDB_JSON_PRINTER_HH

#include <memory>
#include <sstream>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace influxdb {
/**
 *  @class json_printer json_printer.hh
 * "com/centreon/broker/influxdb/json_printer.hh"
 *  @brief Json printer stream.
 *
 *  Print json.
 */
class json_printer {
 public:
  json_printer();
  ~json_printer();
  json_printer(json_printer const&);
  json_printer& operator=(json_printer const&);

  void clear();
  std::string const& get_data() const;
  size_t get_size() const;

  inline void add_tag(std::string const& name) {
    if (!name.empty())
      _data.append("\"").append(name).append("\":");
  }

  json_printer& open_object(std::string const& name = std::string());
  json_printer& close_object();
  json_printer& open_array(std::string const& name);
  json_printer& close_array();

  /**
   *  Add a string value.
   *
   *  @param[in] name   The name of the value.
   *  @param[in] value  The value.
   *
   *  @return           A reference to this object.
   */
  template <typename T>
  json_printer& add_string(std::string const& name, T const& value) {
    add_tag(name);
    std::stringstream ss;
    ss << value;
    _data.append("\"").append(ss.str()).append("\",");
    return (*this);
  }

  /**
   *  Add value.
   *
   *  @param[in] name   The name of the value.
   *  @param[in] value  The value.
   *
   *  @return           A reference to this object.
   */
  template <typename T>
  json_printer& add_value(std::string const& name, T const& value) {
    add_tag(name);
    std::stringstream ss;
    ss << value << ",";
    _data.append(ss.str());
    return (*this);
  }

 private:
  std::string _data;
};
}  // namespace influxdb

CCB_END()

#endif  // !CCB_INFLUXDB_INFLUXDB_JSON_PRINTER_HH
