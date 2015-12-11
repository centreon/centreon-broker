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

static int on_null(void* ctx) {
  static_cast<yajl_visitable*>(ctx)->on_null();
  return (1);
}

static int on_bool(void* ctx, int boolean) {
  static_cast<yajl_visitable*>(ctx)->on_bool(boolean);
  return (1);
}

static int on_number(void* ctx, char const* s, size_t l) {
  static_cast<yajl_visitable*>(ctx)->on_number(std::string(s, l));
  return (1);
}

static int on_string(
             void* ctx,
             unsigned char const* str,
             size_t str_len) {
  static_cast<yajl_visitable*>(ctx)->on_string(std::string(
                                                 (char const*)str,
                                                 str_len));
  return (1);
}

static int on_map_key(
             void* ctx,
             unsigned char const* str,
             size_t str_len) {
  static_cast<yajl_visitable*>(ctx)->on_map_key(std::string(
                                                 (char const*)str,
                                                 str_len));
  return (1);
}

static int on_start_map(void* ctx) {
  static_cast<yajl_visitable*>(ctx)->on_start_map();
  return (1);
}


static int on_end_map(void* ctx) {
  static_cast<yajl_visitable*>(ctx)->on_end_map();
  return (1);
}

static int on_start_array(void* ctx) {
  static_cast<yajl_visitable*>(ctx)->on_start_array();
  return (1);
}

static int on_end_array(void* ctx) {
  static_cast<yajl_visitable*>(ctx)->on_end_array();
  return (1);
}

static ::yajl_callbacks callbacks = {
  on_null,
  on_bool,
  NULL,
  NULL,
  on_number,
  on_string,
  on_start_map,
  on_map_key,
  on_end_map,
  on_start_array,
  on_end_array
};

/**
 *  Constructor.
 *
 *  @param[in] ajl_visitable  The visitable to use.
 */
yajl_parser::yajl_parser(ajl_visitable& visitable)
  : _visitable(visitable) {
  _handle = ::yajl_alloc(&callbacks, NULL, &visitable);
}

/**
 *  Destructor.
 */
yajl_parser::~yajl_parser() {
  ::yajl_free(_handle);
}

/**
 *  Feed data to the parser, try to parse as much as possible.
 *
 *  @param[in] to_append  The string to append.
 */
void yajl_parser::feed(std::string const& to_append) {
  _full_text.append(to_append);
  ::yajl_status retvalue;
  try {
     retvalue = ::yajl_parse(
                  _handle,
                  (const unsigned char*)(to_append.c_str()),
                  to_append.size());
  } catch (std::exception const& e) {
    throw (exceptions::msg() << "parser error: " << e.what());
  }

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

/**
 *  Finish the parsing, assume all data is available.
 */
void yajl_parser::finish() {
  ::yajl_status retvalue;
  try {
  retvalue = ::yajl_complete_parse(_handle);
  } catch (std::exception const& e) {
    throw (exceptions::msg() << "parser error: " << e.what());
  }

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

/**
 *  Get the text.
 *
 *  @return  The text.
 */
std::string const& yajl_parser::get_text() const throw() {
  return (_full_text);
}

/**
 *  Get the target visitable.
 *
 *  @return  The target visitable.
 */
yajl_visitable& yajl_parser::get_target() throw() {
  return (_visitable);
}
