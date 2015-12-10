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

#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/json/yajl_parser.hh"

using namespace com::centreon::broker::json;

/**
 *  Constructor.
 *
 *  @param[in] callbacks  The callbacks to use.
 */
yajl_parser::yajl_parser(yajl_callbacks const& callbacks) {
  _handle = ::yajl_alloc(&callbacks, NULL, this);
}

/**
 *  Destructor.
 */
yajl_parser::~yajl_parser() {
  ::yajl_free(_handle);
}

void yajl_parser::feed(std::string const& to_append) {
  _full_text.append(to_append);
  ::yajl_status retvalue
    = ::yajl_parse(
        _handle,
        (const unsigned char*)(to_append.c_str()),
        to_append.size());
  if (retvalue == ::yajl_status_error) {
    unsigned char* error = ::yajl_get_error(
                            _handle,
                            1,
                            (const unsigned char*)(_full_text.c_str()),
                            _full_text.size());
    std::string error_string((char*)error);
    ::yajl_free_error(_handle, error);
    throw (exceptions::msg() << "parser error: " << error_string);
  }
}

void yajl_parser::finish() {
  ::yajl_status retvalue = ::yajl_complete_parse(_handle);
  if (retvalue == ::yajl_status_error) {
    unsigned char* error = ::yajl_get_error(
                            _handle,
                            1,
                            (const unsigned char*)(_full_text.c_str()),
                            _full_text.size());
    std::string error_string((char*)error);
    ::yajl_free_error(_handle, error);
    throw (exceptions::msg() << "parser error: " << error_string);
  }
}
