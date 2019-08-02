/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_CONFIG_PARSER_HH
#  define CCB_CONFIG_PARSER_HH

#  include <json11.hpp>
#  include <QList>
#  include <QString>
#  include "com/centreon/broker/config/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       config {
  /**
   *  @class parser parser.hh "com/centreon/broker/config/parser.hh"
   *  @brief Parse configuration file.
   *
   *  Parse a configuration file and generate appropriate objects for further
   *  handling.
   */
  class         parser {
  public:
                parser();
                parser(parser const& other);
                ~parser();
    parser&     operator=(parser const& other);
    void        parse(std::string const& file, state& s);
    static bool parse_boolean(QString const& value);

  private:
    void        _parse_endpoint(json11::Json const& elem, endpoint& e);
    void        _parse_logger(json11::Json const& elem, logger& l);
  };
}

CCB_END()

#endif // !CCB_CONFIG_PARSER_HH
