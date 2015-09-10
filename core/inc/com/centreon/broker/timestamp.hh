/*
** Copyright 2012 Centreon
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
#  define CCB_TIMESTAMP_HH

#  include <ctime>
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
   *  Is this timestamp null?
   *
   *  @return  True if this timestamp is null.
   */
  bool is_null() const {
    return (_sec == (time_t)-1);
  }

  // Data.
  std::time_t _sec;
};

CCB_END()

#endif // !CCB_TIMESTAMP_HH
