/*
** Copyright 2014 Merethis
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

#ifndef CCB_PERSISTENT_CACHE_HH
#  define CCB_PERSISTENT_CACHE_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class persistent_cache persistent_cache.hh "com/centreon/broker/persistent_cache.hh"
 *  @brief Provide a generic persistent cache mechanism.
 *
 *  Provide a generic persistent cache used by some modules to store
 *  data on disk between restarts.
 */
class               persistent_cache {
public:
                    persistent_cache();
                    ~persistent_cache();

private:
                    persistent_cache(persistent_cache const& other);
  persistent_cache& operator=(persistent_cache const& other);
};

CCB_END()

#endif // !CCB_PERSISTENT_CACHE_HH
