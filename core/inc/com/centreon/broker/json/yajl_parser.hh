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

#ifndef CCB_JSON_YAJL_PARSER_HH
#  define CCB_JSON_YAJL_PARSER_HH

#  include <string>
#  include "com/centreon/broker/json/yajl/yajl/yajl_parse.h"
#  include "com/centreon/broker/json/yajl_visitable.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace json {
  /**
   *  @class yajl_parser yajl_parser.hh "com/centreon/json/yajl_parser.hh"
   *  @brief Simple yajl parser.
   */
  class                    yajl_parser {
  public:
                           yajl_parser(
                             yajl_visitable& visitable);
                           ~yajl_parser();

    void                   feed(std::string const& to_append);
    void                   finish();
    std::string const&     get_text() const throw();
    yajl_visitable&        get_target() throw();

  private:
    std::string            _stream;

                           yajl_parser(yajl_parser const&);
    yajl_parser&           operator=(yajl_parser const&);

    yajl_visitable&        _visitable;
    std::string            _full_text;
    yajl_handle            _handle;
  };
} // namespace json

CCB_END()

#endif // !CCB_JSON_YAJL_PARSER_HH
