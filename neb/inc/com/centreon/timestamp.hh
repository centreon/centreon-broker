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
 public:
  timestamp(time_t secs = 0, int usecs = 0);
  timestamp(timestamp const& right);
  ~timestamp() throw();
  timestamp& operator=(timestamp const& right);
  bool operator==(timestamp const& right) const throw();
  bool operator!=(timestamp const& right) const throw();
  bool operator<(timestamp const& right) const throw();
  bool operator<=(timestamp const& right) const throw();
  bool operator>(timestamp const& right) const throw();
  bool operator>=(timestamp const& right) const throw();
  timestamp operator+(timestamp const& right) const;
  timestamp operator-(timestamp const& right) const;
  timestamp& operator+=(timestamp const& right);
  timestamp& operator-=(timestamp const& right);
  void add_mseconds(long msecs);
  void add_seconds(time_t secs);
  void add_useconds(long usecs);
  void clear() throw();
  static timestamp max_time() throw();
  static timestamp min_time() throw();
  static timestamp now() throw();
  void sub_mseconds(long msecs);
  void sub_seconds(time_t secs);
  void sub_useconds(long usecs);
  long long to_mseconds() const throw();
  time_t to_seconds() const throw();
  long long to_useconds() const throw();

 private:
  void _internal_copy(timestamp const& right);
  static void _transfer(time_t* secs, unsigned int* usecs);

  time_t _secs;
  unsigned int _usecs;
};

CC_END()

#endif  // !CC_TIMESTAMP_HH
