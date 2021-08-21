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

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <fstream>

using namespace com::centreon::broker::misc;

static char const* whitespaces(" \t\r\n");

/**
 *  Trim a string.
 *
 *  @param[in] str The string.
 *
 *  @return The trimming stream.
 */
std::string& string::trim(std::string& str) noexcept {
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

std::list<fmt::string_view> string::split_sv(std::string const& str, char sep) {
  std::list<fmt::string_view> retval;
  size_t pos = 0, new_pos;

  while (pos != std::string::npos) {
    new_pos = str.find(sep, pos);
    if (new_pos != std::string::npos) {
      retval.emplace_back(str.data() + pos, new_pos - pos);
      pos = new_pos + 1;
    } else {
      retval.emplace_back(str.data() + pos, str.size() - pos);
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

/**
 * @brief Checks if the string given as parameter is a real UTF-8 string.
 * If it is not, it tries to convert it to UTF-8. Encodings correctly changed
 * are ISO-8859-15 and CP-1252.
 *
 * @param str The string to check
 *
 * @return The string itself or a new string converted to UTF-8. The output
 * string should always be an UTF-8 string.
 */
std::string string::check_string_utf8(std::string const& str) noexcept {
  uint32_t val;
  std::string::const_iterator it;
  for (it = str.begin(); it != str.end();) {
    val = (*it & 0xff);
    if ((val & 0x80) == 0) {
      ++it;
      continue;
    }
    val = (val << 8) | (*(it + 1) & 0xff);
    if ((val & 0xe0c0) == 0xc080) {
      val &= 0x1e00;
      if (val == 0)
        break;
      it += 2;
      continue;
    }

    val = (val << 8) | (*(it + 2) & 0xff);
    if ((val & 0xf0c0c0) == 0xe08080) {
      val &= 0xf2000;
      if (val == 0 || val == 0xd2000)
        break;
      it += 3;
      continue;
    }

    val = (val << 8) | (*(it + 3) & 0xff);
    if ((val & 0xf8c0c0c0) == 0xF0808080) {
      val &= 0x7300000;
      if (val == 0 || val > 0x4000000)
        break;
      it += 4;
      continue;
    }
    break;
  }

  if (it == str.end())
    return str;

  /* Not an UTF-8 string */
  bool is_cp1252 = true, is_iso8859 = true;
  auto itt = it;

  auto iso8859_to_utf8 = [&str, &it]() -> std::string {
    /* Strings are both cp1252 and iso8859-15 */
    std::string out;
    std::size_t d = it - str.begin();
    out.reserve(d + 2 * (str.size() - d));
    out = str.substr(0, d);
    while (it != str.end()) {
      uint8_t c = static_cast<uint8_t>(*it);
      if (c < 128)
        out.push_back(c);
      else if (c >= 128 && c <= 160)
        out.push_back('_');
      else {
        switch (c) {
          case 0xa4:
            out.append("€");
            break;
          case 0xa6:
            out.append("Š");
            break;
          case 0xa8:
            out.append("š");
            break;
          case 0xb4:
            out.append("Ž");
            break;
          case 0xb8:
            out.append("ž");
            break;
          case 0xbc:
            out.append("Œ");
            break;
          case 0xbd:
            out.append("œ");
            break;
          case 0xbe:
            out.append("Ÿ");
            break;
          default:
            out.push_back(0xc0 | c >> 6);
            out.push_back((c & 0x3f) | 0x80);
            break;
        }
      }
      ++it;
    }
    return out;
  };
  do {
    uint8_t c = *itt;
    /* not ISO-8859-15 */
    if (c > 126 && c < 160)
      is_iso8859 = false;
    /* not cp1252 */
    if (c & 128)
      if (c == 129 || c == 141 || c == 143 || c == 144 || c == 155)
        is_cp1252 = false;
    if (!is_cp1252)
      return iso8859_to_utf8();
    else if (!is_iso8859) {
      std::string out;
      std::size_t d = it - str.begin();
      out.reserve(d + 3 * (str.size() - d));
      out = str.substr(0, d);
      while (it != str.end()) {
        c = *it;
        if (c < 128)
          out.push_back(c);
        else {
          switch (c) {
            case 128:
              out.append("€");
              break;
            case 129:
            case 141:
            case 143:
            case 144:
            case 157:
              out.append("_");
              break;
            case 130:
              out.append("‚");
              break;
            case 131:
              out.append("ƒ");
              break;
            case 132:
              out.append("„");
              break;
            case 133:
              out.append("…");
              break;
            case 134:
              out.append("†");
              break;
            case 135:
              out.append("‡");
              break;
            case 136:
              out.append("ˆ");
              break;
            case 137:
              out.append("‰");
              break;
            case 138:
              out.append("Š");
              break;
            case 139:
              out.append("‹");
              break;
            case 140:
              out.append("Œ");
              break;
            case 142:
              out.append("Ž");
              break;
            case 145:
              out.append("‘");
              break;
            case 146:
              out.append("’");
              break;
            case 147:
              out.append("“");
              break;
            case 148:
              out.append("”");
              break;
            case 149:
              out.append("•");
              break;
            case 150:
              out.append("–");
              break;
            case 151:
              out.append("—");
              break;
            case 152:
              out.append("˜");
              break;
            case 153:
              out.append("™");
              break;
            case 154:
              out.append("š");
              break;
            case 155:
              out.append("›");
              break;
            case 156:
              out.append("œ");
              break;
            case 158:
              out.append("ž");
              break;
            case 159:
              out.append("Ÿ");
              break;
            default:
              out.push_back(0xc0 | c >> 6);
              out.push_back((c & 0x3f) | 0x80);
              break;
          }
        }
        ++it;
      }
      return out;
      break;
    }
    ++itt;
  } while (itt != str.end());
  assert(is_cp1252 == is_iso8859);
  return iso8859_to_utf8();
}

/**
 * @brief This function adjusts the given integer s so that the str string may
 * be cut at this length and still be a UTF-8 string (we don't want to cut it
 * in a middle of a character).
 *
 * This function assumes the string to be UTF-8 encoded.
 *
 * @param str A string to truncate.
 * @param s The desired size, maybe the resulting string will contain less
 * characters.
 *
 * @return The newly computed size.
 */
size_t string::adjust_size_utf8(const std::string& str, size_t s) {
  if (s >= str.size())
    return str.size();
  if (s == 0)
    return s;
  else {
    while ((str[s] & 0xc0) == 0x80)
      s--;
    return s;
  }
}

/**
 * @brief Escape the given string so that it can be directly inserted into the
 * database. Essntially, characters \ and ' are prefixed with \. The function
 * also only keeps the s first characters.
 *
 * @param str the string to escape and truncate.
 * @param s The desired size.
 *
 * @return The resulting string.
 */
std::string string::escape(const std::string& str, size_t s) {
  size_t found = str.find_first_of("'\\");
  if (found == std::string::npos)
    return str.substr(0, adjust_size_utf8(str, s));
  else {
    std::string ret;
    /* ret is reserved with the worst size */
    ret.reserve(found + 2 * (str.size() - found));
    std::copy(str.data(), str.data() + found, std::back_inserter(ret));
    ret += '\\';
    ret += str[found];
    do {
      ++found;
      size_t ffound = str.find_first_of("'\\", found);
      if (ffound == std::string::npos) {
        std::copy(str.data() + found, str.data() + str.size(),
                  std::back_inserter(ret));
        break;
      }
      std::copy(str.data() + found, str.data() + ffound,
                std::back_inserter(ret));
      ret += '\\';
      ret += str[ffound];
      found = ffound;
    } while (found < s);
    ret.resize(adjust_size_utf8(ret, s));
    if (ret.size() > 1) {
      auto it = --ret.end();
      size_t nb{0};
      while (it != ret.begin() && *it == '\\') {
        --it;
        nb++;
      }
      if (it == ret.begin() && *it == '\\')
        nb++;
      if (nb & 1)
        ret.resize(ret.size() - 1);
    }
    return ret;
  }
}

/**
 * @brief This function is very useful to debug. It displays
 * the data array as hex 8 bits integers in the limit of 20 values. If the array
 * is longer, only the 10 first bytes and the 10 last bytes are displayed.
 *
 * @param data A const char* array.
 * @param size The size of the data array.
 *
 * @return A string containing the result.
 */
std::string string::from_buffer(const char* data, int32_t size) {
  auto to_str = [](uint8_t d) -> uint8_t {
    uint8_t ret;
    if (d < 10)
      ret = '0' + d;
    else
      ret = 'a' + d - 10;
    return ret;
  };

  std::string retval;
  int l1;
  if (size <= 10)
    l1 = size;
  else
    l1 = 10;

  for (int i = 0; i < l1; i++) {
    uint8_t c = data[i];
    uint8_t d1 = c >> 4;
    uint8_t d2 = c & 0xf;
    retval.push_back(to_str(d1));
    retval.push_back(to_str(d2));
  }
  if (size > 10) {
    if (size > 20)
      retval += "...";
    for (int i = std::max(size - 10, l1); i < size; i++) {
      uint8_t c = data[i];
      uint8_t d1 = c >> 4;
      uint8_t d2 = c & 0xf;
      retval.push_back(to_str(d1));
      retval.push_back(to_str(d2));
    }
  }
  return retval;
}
