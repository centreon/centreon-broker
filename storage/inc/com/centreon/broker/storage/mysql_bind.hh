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

#ifndef CCB_STORAGE_MYSQL_BIND_HH
#  define CCB_STORAGE_MYSQL_BIND_HH

#include <mysql.h>
#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                     storage {
  class                       mysql_bind {
   public:
                              mysql_bind(int size);
                              mysql_bind(mysql_bind const& other);
                              ~mysql_bind();
    void                      set_size(int size);
    void                      set_int(int range, int value);
    void                      set_tiny(int range, char value);
    void                      set_uint(int range, unsigned int value);
    void                      set_string(int range, std::string const& value);
    void                      set_float(int range, float value);

    MYSQL_BIND const*         get_bind() const;
   private:
    static my_bool            _true;
    std::vector<MYSQL_BIND>   _bind;

    // The buffers contained by _bind
    std::vector<std::string>  _buffer;

    // The buffers lengths contained by _bind
    std::vector<unsigned long int> _length;
  };
}

CCB_END()

#endif  //CCB_STORAGE_MYSQL_BIND_HH
