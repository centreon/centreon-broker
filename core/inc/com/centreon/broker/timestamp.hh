/*
** Copyright 2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_TIMESTAMP_HH
#  define CCB_TIMESTAMP_HH

#  include <ctime>
#  include <istream>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class timestamp timestamp.hh "com/centreon/broker/timestamp.hh"
 *  @brief Timestamp class.
 *
 *  Holds the time.
 */
struct        timestamp {
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
              timestamp(timestamp const& right) : _sec(right._sec) {}

  /**
   *  Destructor.
   */
              ~timestamp() {}

  /**
   *  Assignment operator.
   *
   *  @param[in] right Object to copy.
   *
   *  @return This object.
   */
  timestamp&  operator=(timestamp const& right) {
    if (this != &right)
      _sec = right._sec;
    return (*this);
  }

  /**
   *  Get timestamp as time_t.
   *
   *  @return Timestamp as time_t.
   */
              operator std::time_t() const {
    return (_sec);
  }

  /**
   *  Get timestamp as time_t.
   *
   *  @return Timestamp as time_t.
   */
  std::time_t get_time_t() const {
    return (_sec);
  }

  /**
   *  Is this a null timestamp ?
   *
   *  @return  True if this is a null timestamp.
   */
  bool is_null() const {
    return (_sec == (time_t)-1);
  }

  /**
   *  Clear the timestamp.
   */
  void clear() {
    _sec = (time_t)-1;
  }

  /**
   *  Comparison function.
   *
   *  @param[in] left   The left object.
   *  @param[in] right  The right object.
   *
   *  @return           True if this object is less than the other.
   */
  static bool less(timestamp const& left, timestamp const& right) {
    if (left.is_null() && !right.is_null())
      return (false);
    else if (!left.is_null() && right.is_null())
      return (true);
    else
      return (left._sec < right._sec);
  }

  // Data.
  std::time_t _sec;
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

#endif // !CCB_TIMESTAMP_HH
