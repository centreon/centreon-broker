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

#ifndef CCB_INFLUXDB_COLUMN_HH
#  define CCB_INFLUXDB_COLUMN_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           influxdb {
  /**
   *  @class column column.hh "com/centreon/broker/influxdb/column.hh"
   *  @brief Store the data for a column in the query.
   */
  class             column {
  public:
    enum            type {
                    string,
                    number
    };

                    column();
                    column(
                      std::string const& name,
                      std::string const& value,
                      bool is_flag,
                      type col_type);
                    column(column const& c);
    column&         operator=(column const& c);

    std::string const&
                    get_name() const;
    std::string const&
                    get_value() const;
    bool            is_flag() const;
    type            get_type() const;

  private:
    std::string     _name;
    std::string     _value;
    bool            _is_flag;
    type            _type;
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_COLUMN_HH
