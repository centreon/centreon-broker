/*
** Copyright 2013 Centreon
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

#ifndef CCB_MISC_STRING_HH
#define CCB_MISC_STRING_HH

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <fmt/format.h>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
namespace string {
inline std::string& replace(std::string& str,
                            std::string const& old_str,
                            std::string const& new_str) {
  std::size_t pos(str.find(old_str, 0));
  while (pos != std::string::npos) {
    str.replace(pos, old_str.size(), new_str);
    pos = str.find(old_str, pos + new_str.size());
  }
  return (str);
}

std::list<std::string> split(std::string const& str, char sep);
std::string& trim(std::string& str) throw();
std::string base64_encode(std::string const& str);
bool is_number(const std::string& s);
std::string check_string_utf8(const std::string& str) noexcept;

/**
 * @brief This function works almost like the resize method but takes care
 * of the UTF-8 encoding and avoids to cut a string in the middle of a
 * character. This function assumes the string to be UTF-8 encoded.
 *
 * @param str A string to truncate.
 * @param s The desired size, maybe the resulting string will contain less
 * characters.
 *
 * @return a reference to the string str.
 */
template<typename T>
fmt::string_view truncate(const T& str, size_t s) {
  if (s >= str.size())
    return fmt::string_view(str);
  if (s > 0)
    while ((str[s] & 0xc0) == 0x80)
      s--;
  return fmt::string_view(str.data(), s);
}

size_t adjust_size_utf8(const std::string& str, size_t s);

}  // namespace string
}  // namespace misc

CCB_END()

#endif  // !CCB_MISC_STRING_HH
