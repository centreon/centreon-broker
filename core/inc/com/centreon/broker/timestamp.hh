/*
** Copyright 2012,2015-2016, 2021 Centreon
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

#ifndef CCB_TIMESTAMP_HH
#define CCB_TIMESTAMP_HH

#include <ctime>
#include <istream>
#include <limits>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class timestamp timestamp.hh "com/centreon/broker/timestamp.hh"
 *  @brief Timestamp class.
 *
 *  Holds the time.
 */
struct timestamp {
  // Data.
  std::time_t _sec;

 public:
  /**
   *  Default constructor.
   *
   *  Time is not defined.
   */
  timestamp() : _sec((time_t)-1) {}

  /**
   *  Build from a time_t.
   *
   *  @param[in] t Time expressed in time_t.
   */
  timestamp(std::time_t t) : _sec(t) {}

  /**
   *  Copy constructor.
   *
   *  @param[in] right Object to copy.
   */
  timestamp(const timestamp& right) : _sec(right._sec) {}

  /**
   *  Destructor.
   */
  ~timestamp() noexcept = default;

  /**
   *  Assignment operator.
   *
   *  @param[in] right Object to copy.
   *
   *  @return This object.
   */
  timestamp& operator=(const timestamp& right) {
    if (this != &right)
      _sec = right._sec;
    return *this;
  }

  /**
   *  Comparison function.
   *
   *  @param[in] other  The right object.
   *
   *  @return           True if this object is less than the other.
   */
  bool operator<(const timestamp& other) const noexcept {
    if (is_null() && !other.is_null())
      return false;
    else if (!is_null() && other.is_null())
      return true;
    else
      return _sec < other._sec;
  }

  /**
   *  Comparison function.
   *
   *  @param[in] other  The right object.
   *
   *  @return           True if this object is greater than the other.
   */
  bool operator>(const timestamp& other) const noexcept {
    if (is_null() && !other.is_null())
      return true;
    else if (!is_null() && other.is_null())
      return false;
    else
      return _sec > other._sec;
  }

  /**
   *  Get timestamp as time_t.
   *
   *  @return Timestamp as time_t.
   */
  operator std::time_t() const { return _sec; }

  /**
   *  Get timestamp as time_t.
   *
   *  @return Timestamp as time_t.
   */
  inline std::time_t get_time_t() const { return _sec; }

  /**
   *  Is this a null timestamp ?
   *
   *  @return  True if this is a null timestamp.
   */
  bool is_null() const { return _sec == (time_t)-1 || _sec == (time_t)0; }

  /**
   *  Clear the timestamp.
   */
  void clear() { _sec = (time_t)-1; }

  /**
   *  Return a timestamp from now.
   *
   *  @return  A timestamp set to present time, present day.
   */
  static timestamp now() { return ::time(nullptr); }

  /**
   *  Return the upper time limit.
   *
   *  @return A timestamp set in a very far future.
   */
  static timestamp max() {
    return timestamp(std::numeric_limits<time_t>::max());
  }
};

/**
 *  Stream operator for timestamp.
 *
 *  @param[in] stream  The stream.
 *  @param[in] ts      The timestamp.
 *
 *  @return            Reference to the stream.
 */
inline std::istream& operator>>(std::istream& stream, timestamp& ts) {
  std::time_t s;
  stream >> s;
  ts = timestamp(s);
  return (stream);
}

CCB_END()

#endif  // !CCB_TIMESTAMP_HH
