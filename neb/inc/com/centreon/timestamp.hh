/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_TIMESTAMP_HH
#  define CC_TIMESTAMP_HH

#  include <ctime>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class timestamp timestamp.hh "com/centreon/timestamp.hh"
 *  @brief Provide time management.
 *
 *  Allow to manage time easily.
 */
class              timestamp {
public:
                   timestamp(time_t secs = 0, int usecs = 0);
                   timestamp(timestamp const& right);
                   ~timestamp() throw ();
  timestamp&       operator=(timestamp const& right);
  bool             operator==(timestamp const& right) const throw ();
  bool             operator!=(timestamp const& right) const throw ();
  bool             operator<(timestamp const& right) const throw ();
  bool             operator<=(timestamp const& right) const throw ();
  bool             operator>(timestamp const& right) const throw ();
  bool             operator>=(timestamp const& right) const throw ();
  timestamp        operator+(timestamp const& right) const;
  timestamp        operator-(timestamp const& right) const;
  timestamp&       operator+=(timestamp const& right);
  timestamp&       operator-=(timestamp const& right);
  void             add_mseconds(long msecs);
  void             add_seconds(time_t secs);
  void             add_useconds(long usecs);
  void             clear() throw ();
  static timestamp max_time() throw ();
  static timestamp min_time() throw ();
  static timestamp now() throw ();
  void             sub_mseconds(long msecs);
  void             sub_seconds(time_t secs);
  void             sub_useconds(long usecs);
  long long        to_mseconds() const throw ();
  time_t           to_seconds() const throw ();
  long long        to_useconds() const throw ();

private:
  void             _internal_copy(timestamp const& right);
  static void      _transfer(time_t* secs, unsigned int* usecs);

  time_t           _secs;
  unsigned int     _usecs;
};

CC_END()

#endif // !CC_TIMESTAMP_HH
