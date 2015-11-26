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

#ifndef CCB_JSON_JSON_PARSER_HH
#  define CCB_JSON_JSON_PARSER_HH

#  include <vector>
#  include <string>
#  include "com/centreon/broker/json/jsmn.h"
#  include "com/centreon/broker/json/json_iterator.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace json {
  /**
   *  @class json_parser json_parser.hh "com/centreon/json/json_parser.hh"
   *  @brief Simple json parser based on jsmn.
   */
  class                    json_parser {
  public:
                           json_parser();
                           ~json_parser();

    void                   parse(std::string const& js);
    json_iterator          begin() const;

  private:
    jsmn_parser            _parser;
    std::string            _js;
    std::vector<jsmntok_t> _tokens;

                           json_parser(json_parser const&);
    json_parser&           operator=(json_parser const&);
  };
} // namespace json

CCB_END()

#endif // !CCB_JSON_JSON_PARSER_HH
