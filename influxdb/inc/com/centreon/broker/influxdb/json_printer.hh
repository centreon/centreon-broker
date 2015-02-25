/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_INFLUXDB_INFLUXDB_JSON_PRINTER_HH
#  define CCB_INFLUXDB_INFLUXDB_JSON_PRINTER_HH

#  include <sstream>
#  include <string>
#  include <memory>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace influxdb {
  /**
   *  @class json_printer json_printer.hh "com/centreon/broker/influxdb/json_printer.hh"
   *  @brief Json printer stream.
   *
   *  Print json.
   */
  class         json_printer {
  public:
                json_printer();
                ~json_printer();
                json_printer(json_printer const&);
  json_printer& operator=(json_printer const&);

  std::string const&
                get_data() const;
  size_t        get_size() const;

  json_printer& open_object(std::string const& name = std::string());
  json_printer& close_object();
  json_printer& open_array(std::string const& name);
  json_printer& close_array();
  json_printer& add_string(std::string const& name, std::string const& value);

  /**
   *  Add a number value.
   *
   *  @param[in] name   The name of the value.
   *  @param[in] value  The value.
   *
   *  @return           A reference to this object.
   */
  template <typename T>
  json_printer& add_number(std::string const& name, T value) {
    std::stringstream ss;
    ss << '"' << name << "\":" << value << ",";
    _data.append(ss.str());
    return (*this);
  }

  private:
    std::string _data;
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_INFLUXDB_JSON_PRINTER_HH
