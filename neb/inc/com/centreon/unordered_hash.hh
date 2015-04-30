/*
** Copyright 2012-2013 Merethis
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

#ifndef CC_UNORDERED_HASH_HH
#  define CC_UNORDERED_HASH_HH

#  if __cplusplus == 201103L
#    define CXX0X_UNORDERED 1
#  elif defined(__clang__)
#    if __has_feature(cxx_variadic_templates)
#      define TR1_UNORDERED 1
#    endif // cxx_variadic_templates.
#  elif defined(__GNUC__) && __GNUC__ >= 4
#    define TR1_UNORDERED 1
#  endif // C++0x, tr1

// Used c++0x implementation.
#  ifdef CXX0X_UNORDERED
#    include <functional>
#    include <unordered_map>
#    include <unordered_set>
#    include <utility>

#    define umap std::unordered_map
#    define umultimap std::unordered_multimap
#    define uset std::unordered_set
#    define umultiset std::unordered_multiset

// Missing std::pair hash.
namespace std {
  template <typename T, typename U>
  struct hash<std::pair<T, U> > {
    std::size_t operator()(std::pair<T, U> const& p) const {
      std::hash<T> h1;
      std::hash<U> h2;
      return (h1(p.first) ^ h2(p.second));
    }
  };
}

// Used tr1 implementation.
#  elif defined(TR1_UNORDERED)
#    include <functional>
#    include <tr1/unordered_map>
#    include <tr1/unordered_set>
#    include <utility>

#    define umap std::tr1::unordered_map
#    define umultimap std::tr1::unordered_multimap
#    define uset std::tr1::unordered_set
#    define umultiset std::tr1::unordered_multiset

// Missing std::pair hash.
namespace std {
  namespace tr1 {
    template <typename T, typename U>
    struct hash<std::pair<T, U> > {
      std::size_t operator()(std::pair<T, U> const& p) const {
        std::tr1::hash<T> h1;
        std::tr1::hash<U> h2;
        return (h1(p.first) ^ h2(p.second));
      }
    };
  }
}

namespace std {
  // Missing equal operator for unrodered map on tr1.
  template<class Key, class T, class Hash, class Pred, class Alloc>
  bool operator==(
         umap<Key, T, Hash, Pred, Alloc> const& lhs,
         umap<Key, T, Hash, Pred, Alloc> const& rhs) {
    if (lhs.size() != rhs.size())
      return (false);
    for (typename umap<Key, T, Hash, Pred, Alloc>::const_iterator
           it(lhs.begin()), end(lhs.end());
         it != end;
         ++it) {
      typename umap<Key, T, Hash, Pred, Alloc>::const_iterator
        it_find(rhs.find(it->first));
      if (it_find == rhs.end() || it_find->second != it->second)
        return (false);
    }
    return (true);
  }
  // Missing not equal operator for unrodered map on tr1.
  template<class Key, class T, class Hash, class Pred, class Alloc>
  bool operator!=(
         umap<Key, T, Hash, Pred, Alloc> const& lhs,
         umap<Key, T, Hash, Pred, Alloc> const& rhs) {
    return (!operator==(lhs, rhs));
  }

  // Missing equal operator for unrodered multimap on tr1.
  template<class Key, class T, class Hash, class Pred, class Alloc>
  bool operator==(
         umultimap<Key, T, Hash, Pred, Alloc> const& lhs,
         umultimap<Key, T, Hash, Pred, Alloc> const& rhs) {
    if (lhs.size() != rhs.size())
      return (false);
    for (typename umap<Key, T, Hash, Pred, Alloc>::const_iterator
           it(lhs.begin()), end(lhs.end());
         it != end;
         ++it) {
      bool find(false);
      for (typename umap<Key, T, Hash, Pred, Alloc>::const_iterator
             it_find(rhs.find(it->first)), end(rhs.end());
           it_find != end && it_find->first == it->first;
           ++it_find) {
        if (it_find->second == it->second) {
          find = true;
          break;
        }
      }
      if (!find)
        return (false);
    }
    return (true);
  }
  // Missing not equal operator for unrodered multimap on tr1.
  template<class Key, class T, class Hash, class Pred, class Alloc>
  bool operator!=(
         umultimap<Key, T, Hash, Pred, Alloc> const& lhs,
         umultimap<Key, T, Hash, Pred, Alloc> const& rhs) {
    return (!operator==(lhs, rhs));
  }
}

// Used std implementation.
#  else
#    include <map>
#    include <set>
#    define umap std::map
#    define umultimap std::multimap
#    define uset std::set
#    define umultiset std::multiset
#  endif // C++0X, tr1 or std

#endif // !CC_UNORDERED_HASH_HH
