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

#ifndef CCB_JSON_YAJL_VISITABLE_HH
#  define CCB_JSON_YAJL_VISITABLE_HH

#  include <string>
#  include "com/centreon/broker/json/yajl/yajl/yajl_parse.h"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace json {
  /**
   *  @class yajl_visitable yajl_visitable.hh "com/centreon/json/yajl_visitable.hh"
   *  @brief YAJL visitable interface.
   */
  class           yajl_visitable {
  public:
    virtual       ~yajl_visitable() {}

    virtual void  on_null() = 0;
    virtual void  on_bool(bool val) = 0;
    virtual void  on_number(std::string const& num) = 0;
    virtual void  on_string(std::string const& string) = 0;
    virtual void  on_start_map() = 0;
    virtual void  on_end_map() = 0;
    virtual void  on_map_key(std::string const& key) = 0;
    virtual void  on_start_array() = 0;
    virtual void  on_end_array() = 0;
  };
} // namespace json

CCB_END()

#endif // !CCB_JSON_YAJL_VISITABLE_HH
