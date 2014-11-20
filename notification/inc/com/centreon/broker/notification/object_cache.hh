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
#  include <vector>
#  include <map>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             notification {
  /**
   *  @class object_cache object_cache.hh "com/centreon/broker/notification/object_cache.hh"
   *  @tparam T  The type of the object cached.
   *  @brief Contain a fixed size cache of a particular object by its id.
   */
  template <typename T>
  class               object_cache {
  public:
    /**
      *  Default constructor
      */
                      object_cache() {}
    /**
     *  Copy constructor
     * @param[in] obj  The object to copy.
     */
                      object_cache(object_cache<T> const& obj) {
      object_cache<T>::operator=(obj);
    }

    /**
     *  Assignment operator.
     *
     *  @param[in] obj  The obect to copy.
     *
     *  @return  A reference this object.
     */
    object_cache<T>&  operator=(object_cache<T> const& obj) {
      if (this != &obj) {
        _cache = obj._cache;
      }
      return (*this);
    }

    /**
     *  Insert an object.
     *
     *  @param[in] id      The id of the object.
     *  @param[in] object  The object to insert.
     */
    void              insert(unsigned int id,
                             T const& object) {
      typename std::map<unsigned int, std::vector<T> >::iterator
          found(_cache.find(id));
      if (found == _cache.end())
        _cache.insert(
            std::make_pair(id, std::vector<T>())).first->second.push_back(object);
      else {
        found->second.push_back(object);
        if (found->second.size() > cache_size)
          found->second.erase(found->second.begin());
      }
    }

    std::vector<T>    get(unsigned int id) {
      typename std::map<unsigned int, std::vector<T> >::iterator
          found(_cache.find(id));
      if (found == _cache.end())
        return (std::vector<T>());
      else
        return (found->second);
    }

    /**
     *  Is the cache empty?
     *
     *  @return  True if the cache is empty.
     */
    bool              empty() const {
      return (_cache.empty());
    }

    /**
     *  Pop an object from the cache.
     *
     *  @return  An object from the cache.
     */
    T                pop() {
      if (_cache.empty())
        return (T());
      else {
        typename std::map<unsigned int, std::vector<T> >::iterator
            begin = _cache.begin();
        T ret = *begin->second.begin();
        begin->second.erase(begin->second.begin());
        if (begin->second.empty())
          _cache.erase(begin);
        return (ret);
      }
    }

    static const int cache_size = 2;

  private:
    std::map<unsigned int, std::vector<T> >
                     _cache;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
