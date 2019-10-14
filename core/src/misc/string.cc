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

#include "com/centreon/broker/misc/string.hh"
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace com::centreon::broker::misc;

static char const* whitespaces(" \t\r\n");

/**
 *  Trim a string.
 *
 *  @param[in] str The string.
 *
 *  @return The trimming stream.
 */
std::string& string::trim(std::string& str) throw() {
  size_t pos(str.find_last_not_of(whitespaces));
  if (pos == std::string::npos)
    str.clear();
  else {
    str.erase(pos + 1);
    if ((pos = str.find_first_not_of(whitespaces)) != std::string::npos)
      str.erase(0, pos);
  }
  return str;
}

std::list<std::string> string::split(std::string const& str, char sep) {
  std::list<std::string> retval;
  size_t pos = 0, new_pos;
  while (pos != std::string::npos) {
    new_pos = str.find(sep, pos);
    if (new_pos != std::string::npos) {
      retval.push_back(str.substr(pos, new_pos - pos));
      pos = new_pos + 1;
    } else {
      retval.push_back(str.substr(pos));
      pos = new_pos;
    }
  }

  return retval;
}

std::string string::base64_encode(const std::string& str) {
  static const std::string b =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string retval;
  retval.reserve((str.size() / 3 + (str.size() % 3 > 0)) * 4);

  int val = 0, valb = -6;
  for (unsigned char c : str) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      retval.push_back(b[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6)
    retval.push_back(b[((val << 8) >> (valb + 8)) & 0x3F]);
  while (retval.size() % 4)
    retval.push_back('=');

  return retval;
}

bool string::is_number(const std::string& s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) {
                         return !std::isdigit(c);
                       }) == s.end();
}
