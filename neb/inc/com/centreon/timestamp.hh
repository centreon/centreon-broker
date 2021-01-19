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

#ifndef CC_TIMESTAMP_HH
#define CC_TIMESTAMP_HH

#include <cstdint>
#include <ctime>
#include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class timestamp timestamp.hh "com/centreon/timestamp.hh"
 *  @brief Provide time management.
 *
 *  Allow to manage time easily.
 */
class timestamp {
 private:
  time_t _secs;
  uint32_t _usecs;

 public:
  timestamp(time_t secs = 0, int32_t usecs = 0);
  timestamp(const timestamp& right);
  ~timestamp() noexcept = default;
  timestamp& operator=(const timestamp& right);
  bool operator==(const timestamp& right) const noexcept;
  bool operator!=(const timestamp& right) const noexcept;
  bool operator<(const timestamp& right) const noexcept;
  bool operator<=(const timestamp& right) const noexcept;
  bool operator>(const timestamp& right) const noexcept;
  bool operator>=(const timestamp& right) const noexcept;
  timestamp operator+(const timestamp& right) const;
  timestamp operator-(const timestamp& right) const;
  timestamp& operator+=(const timestamp& right);
  timestamp& operator-=(const timestamp& right);
  void add_mseconds(int32_t msecs);
  void add_seconds(time_t secs) noexcept;
  void add_useconds(int32_t usecs);
  void clear() noexcept;
  static timestamp max_time() noexcept;
  static timestamp min_time() noexcept;
  static timestamp now() noexcept;
  void sub_mseconds(int32_t msecs);
  void sub_seconds(time_t secs) noexcept;
  void sub_useconds(int32_t usecs);
  int64_t to_mseconds() const noexcept;
  time_t to_seconds() const noexcept;
  int64_t to_useconds() const noexcept;
};

CC_END()

#endif  // !CC_TIMESTAMP_HH
