/*
** Copyright 2011-2014 Centreon
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

#include "com/centreon/broker/json/json_parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::json;

/**
 *  Constructor.
 */
json_parser::json_parser() {}

/**
 *  Destructor.
 */
json_parser::~json_parser() {}

/**
 *  Parse a json string.
 *
 *  @param[in] js  The json string.
 */
void json_parser::parse(std::string const& js) {
  ::jsmn_init(&_parser);

  // Get the number of tokens.
  jsmnerr_t ret = ::jsmn_parse(&_parser, js.c_str(), js.size(), NULL, 0);

  // An error has occured.
  if (ret < 0) {
    if (ret == JSMN_ERROR_INVAL)
      throw (exceptions::msg()
             << "couldn't parse json '"
             << js << "': invalid character inside js string");
    else if (ret == JSMN_ERROR_PART)
      throw (exceptions::msg()
             << "couldn't parse json '"
             << js << "': unexpected termination");
    else
      throw (exceptions::msg()
             << "couldn't parse json '"
             << js << "': unknown error");
  }

  _tokens.clear();
  _tokens.resize(ret);

  // Parse the tokens.
  ::jsmn_init(&_parser);
  ::jsmn_parse(&_parser, js.c_str(), js.size(), &_tokens[0], _tokens.size());

  _js = js;
}

/**
 *  Create an iterator used to iterate over all the tokens of
 *  this json document.
 *
 *  @return  The iterator.
 */
json_iterator json_parser::begin() const {
  return (json_iterator(_js.c_str(), &_tokens[0], _tokens.size()));
}
