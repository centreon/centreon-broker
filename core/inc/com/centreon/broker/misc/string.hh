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

bool get_next_line(std::ifstream& stream, std::string& line, uint32_t& pos);
bool split(std::string& line, char const** key, char const** value, char delim);
bool split(std::string const& line,
           std::string& key,
           std::string& value,
           char delim);
void split(std::string const& data, std::list<std::string>& out, char delim);
void split(std::string const& data, std::vector<std::string>& out, char delim);
std::list<std::string> split(std::string const& str, char sep);
std::string& trim(std::string& str) throw();
std::string& trim_left(std::string& str) throw();
std::string& trim_right(std::string& str) throw();
std::string base64_encode(std::string const& str);
bool is_number(const std::string& s);
void replace_all(std::string& input,
                 std::string const& str,
                 std::string const& str2);

inline char const* chkstr(char const* str) throw() {
  return (str ? str : "\"NULL\"");
}

inline std::string ctime(time_t const& time) {
  char buf[64];
  buf[0] = 0;
  if (ctime_r(&time, buf))
    buf[strlen(buf) - 1] = 0;
  return (buf);
}

template <typename T>
inline std::string get(T value) {
  std::ostringstream oss;
  oss << value;
  return (oss.str());
}

inline char* dup(char const* value) {
  if (!value)
    return (NULL);
  char* buf(new char[strlen(value) + 1]);
  return (strcpy(buf, value));
}

inline char* dup(std::string const& value) {
  char* buf(new char[value.size() + 1]);
  return (strcpy(buf, value.c_str()));
}

template <typename T>
inline char* dup(T value) {
  std::ostringstream oss;
  oss << value;
  std::string const& str(oss.str());
  char* buf(new char[str.size() + 1]);
  strcpy(buf, str.c_str());
  return (buf);
}

inline char const* setstr(char*& buf, char const* value = NULL) {
  delete[] buf;
  return ((buf = string::dup(value)));
}

template <typename T>
inline char const* setstr(char*& buf, T value) {
  delete[] buf;
  return ((buf = string::dup(value)));
}

template <typename T>
inline bool to(char const* str, T& data) {
  std::istringstream iss(str);
  return ((iss >> data) && iss.eof());
}

template <>
inline bool to(char const* str, long& data) {
  char* end(NULL);
  errno = 0;
  data = strtol(str, &end, 10);
  return (!*end && !errno);
}

template <>
inline bool to(char const* str, unsigned long& data) {
  char* end(NULL);
  errno = 0;
  data = strtoul(str, &end, 10);
  return (!*end && !errno);
}

template <>
inline bool to(char const* str, bool& data) {
  unsigned long tmp;
  if (!to(str, tmp))
    return (false);
  data = static_cast<bool>(tmp);
  return (true);
}

template <>
inline bool to(char const* str, double& data) {
  char* end(NULL);
  errno = 0;
  data = strtod(str, &end);
  return (!*end && !errno);
}

template <>
inline bool to(char const* str, float& data) {
  char* end(NULL);
  errno = 0;
  data = strtof(str, &end);
  return (!*end && !errno);
}

template <>
inline bool to(char const* str, int& data) {
  long tmp;
  if (!to(str, tmp) || tmp > std::numeric_limits<int>::max() ||
      tmp < std::numeric_limits<int>::min())
    return (false);
  data = static_cast<int>(tmp);
  return (true);
}

template <>
inline bool to(char const* str, long double& data) {
  char* end(NULL);
  errno = 0;
  data = strtold(str, &end);
  return (!*end && !errno);
}

template <>
inline bool to(char const* str, long long& data) {
  char* end(NULL);
  errno = 0;
  data = strtoll(str, &end, 10);
  return (!*end && !errno);
}

template <>
inline bool to(char const* str, uint32_t& data) {
  unsigned long tmp;
  if (!to(str, tmp) || tmp > std::numeric_limits<uint32_t>::max())
    return (false);
  data = static_cast<uint32_t>(tmp);
  return (true);
}

template <>
inline bool to(char const* str, unsigned long long& data) {
  char* end(NULL);
  errno = 0;
  data = strtoull(str, &end, 10);
  return (!*end && !errno);
}

template <typename T, typename U>
inline bool to(char const* str, U& data) {
  T tmp;
  if (!to(str, tmp))
    return (false);
  data = static_cast<U>(tmp);
  return (true);
}

}  // namespace string
}  // namespace misc

CCB_END()

#endif  // !CCB_MISC_STRING_HH
