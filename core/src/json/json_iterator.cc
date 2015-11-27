/*
** Copyright 2011-2015 Centreon
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
#include "com/centreon/broker/json/json_iterator.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::json;

/**
 *  Constructor.
 *
 *  @param[in] js            The json string.
 *  @param[in] tokens        Parsed tokens.
 *  @param[in] token_number  Number of parsed tokens.
 */
json_iterator::json_iterator(
                 const char* js,
                 const jsmntok_t* tokens,
                 size_t token_number)
  : _js(js),
    _tokens(tokens),
    _token_number(token_number),
    _index(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
json_iterator::json_iterator(json_iterator const& other)
  : _js(other._js),
    _tokens(other._tokens),
    _token_number(other._token_number),
    _index(other._index) {}

/**
 *  Assignment operator.
 *
 *  @return
 */
json_iterator& json_iterator::operator=(json_iterator const& other) {
  if (this != &other) {
    _js = other._js;
    _tokens = other._tokens;
    _token_number = other._token_number;
    _index = other._index;
  }
  return (*this);
}

/**
 *  Destructor.
 */
json_iterator::~json_iterator() {}

/**
 *  Go forward a node. Do not enter child nodes.
 *
 *  @return  A reference to this iterator.
 */
json_iterator& json_iterator::operator++() {
  if (end())
    return (*this);
  int parent = _tokens[_index].parent;
  for (_index = _index + 1;
       _index < _token_number && _tokens[_index].parent != parent;
       ++_index);
  return (*this);
}

/**
 *  Get the type of this node.
 *
 *  @return The type of this node.
 */
json_iterator::type json_iterator::get_type() const throw() {
  if (end())
    return (null);
  switch (_tokens[_index].type) {
    case JSMN_OBJECT:
      return (object);
    case JSMN_ARRAY:
      return (array);
    case JSMN_STRING:
      return (string);
    case JSMN_PRIMITIVE:
      switch (_js[_tokens[_index].start]) {
        case 't':
        case 'f':
          return (boolean);
        case 'n':
          return (null);
        default:
          return (number);
      }
    default:
      return (null);
  }
}

/**
 *  Get the type of this node as a string.
 *
 *  @return  The type of this node as a string.
 */
std::string json_iterator::get_string_type() const {
  type t = get_type();

  switch (t) {
  case object:
    return ("object");
  case array:
    return ("array");
  case string:
    return ("string");
  case number:
    return ("number");
  case boolean:
    return ("boolean");
  case null:
  default:
    return ("null");
  }
}

/**
 *  Get this node as a string.
 *
 *  @return  This node as a string.
 */
std::string json_iterator::get_string() const {
  return (end()
            ? ""
            : std::string(
              &_js[_tokens[_index].start],
              _tokens[_index].end - _tokens[_index].start));
}

/**
 *  Get this node as an integer.
 *
 *  @return  This node as an integer.
 */
int json_iterator::get_integer() const {
  std::stringstream ss;
  ss << get_string();
  int ret;
  ss >> ret;
  if (ss.fail())
    throw (exceptions::msg()
           << "couldn't parse '"
           << get_string() << "' into an integer");
  return (ret);
}

/**
 *  Get this node as a boolean.
 *
 *  @return  The value of this boolean.
 */
bool json_iterator::get_bool() const {
  std::string str = get_string();
  return (str == "true" ? true : false);
}

/**
 *  Is this node null?
 *
 *  @return  True if this node is null.
 */
bool json_iterator::is_null() const throw() {
  return (get_type() == null);
}

/**
 *  Get the number of children of this node.
 *
 *  @return  Number of children of this node.
 */
int json_iterator::children() const throw() {
  return (_tokens[_index].size);
}

/**
 *  Go in the children, if any.
 *
 *  @return  A json_iterator on the first children, if any.
 */
json_iterator json_iterator::enter_children() const throw() {
  if (_tokens[_index].size > 0)
    return (json_iterator(
              _js,
              _tokens + _index + 1,
              _token_number - _index - 1));
  else
    return (json_iterator(_js, _tokens, 0));
}

/**
 *  Find a child.
 *
 *  @param[in] name  The name of the child.
 *
 *  @return          Iterator this child, if any.
 */
json_iterator json_iterator::find_child(std::string const& name) const {
  json_iterator ret = this->enter_children();
  for (; !ret.end(); ++ret)
    if (ret.get_string() == name)
      return (ret);
  return (ret);
}

/**
 *  Did this iterator end?
 *
 *  @return  True if this iterator is at the end.
 */
bool json_iterator::end() const throw() {
  return (_index >= _token_number);
}
