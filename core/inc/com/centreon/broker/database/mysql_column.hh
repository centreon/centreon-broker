/*
** Copyright 2018 Centreon
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

#ifndef CCB_DATABASE_MYSQL_COLUMN_HH
#  define CCB_DATABASE_MYSQL_COLUMN_HH

#  include <cmath>
#  include <mysql.h>
#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             database {
  class                 mysql_column {
   public:
                        mysql_column(int type = MYSQL_TYPE_LONG, int row_count = 1, int length = 0);
                        mysql_column(mysql_column&& other);
    mysql_column&       operator=(mysql_column const& other);
                        ~mysql_column();
    int                 get_type() const;
    void*               get_buffer();
    void                set_length(int len);
    void                set_type(int type);

    template<typename T>
    void                set_value(T value) {
      T* vector(static_cast<T*>(_vector));
      vector[0] = value;
    }

    void                set_value(std::string const& str);
    my_bool*            is_null_buffer();
    bool                is_null() const;
    my_bool*            error_buffer();
    unsigned long*      length_buffer();

   private:
    int                 _type;
    int                 _row_count;
    unsigned int        _str_size;
    void*               _vector;
    std::vector<my_bool>
                        _is_null;
    std::vector<my_bool>
                        _error;
    std::vector<unsigned long>
                        _length;
  };

  template<>
  inline void mysql_column::set_value<double>(double val) {
    double* vector(static_cast<double*>(_vector));
    _is_null[0] = (std::isnan(val) || std::isinf(val));
    vector[0] = val;
  }

  template<>
  inline void mysql_column::set_value<float>(float val) {
    float* vector(static_cast<float*>(_vector));
    _is_null[0] = (std::isnan(val) || std::isinf(val));
    vector[0] = val;
  }

}

CCB_END()

#endif  //CCB_DATABASE_MYSQL_COLUMN_HH
