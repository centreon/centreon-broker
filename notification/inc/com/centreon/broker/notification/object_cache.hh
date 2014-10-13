/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_OBJECT_CACHE_HH
#  define CCB_NOTIFICATION_OBJECT_CACHE_HH

#  include <string>
#  include <queue>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             notification {
  /**
   *  @class object_cache object_cache.hh "com/centreon/broker/notification/object_cache.hh"
   *  @tparam T  The type of the object cached.
   *  @brief Contain a cache of a particular neb object.
   */
  template <typename T>
  class               object_cache {
  public:
                      object_cache() {}
                      object_cache(object_cache<T> const& obj) {
      object_cache<T>::operator=(obj);
    }
    object_cache<T>&  operator=(object_cache<T> const& obj) {
      if (this != &obj) {
        _cache = obj._cache;
      }
      return (*this);
    }

    void              push(T const& object) {
      _cache.push(object);
      if (_cache.size() > cache_size)
        _cache.pop();
    }

    static const int cache_size = 2;

  private:
    std::queue<T>    _cache;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
