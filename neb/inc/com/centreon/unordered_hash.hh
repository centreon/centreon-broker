/*
** Copyright 2012-2013 Centreon
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

#ifndef CC_UNORDERED_HASH_HH
#define CC_UNORDERED_HASH_HH

#if __cplusplus == 201103L
#define CXX0X_UNORDERED 1
#elif defined(__clang__)
#if __has_feature(cxx_variadic_templates)
#define TR1_UNORDERED 1
#endif  // cxx_variadic_templates.
#elif defined(__GNUC__) && __GNUC__ >= 4
#define TR1_UNORDERED 1
#endif  // C++0x, tr1

// Used c++0x implementation.
#ifdef CXX0X_UNORDERED
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#define umap std::unordered_map
#define umultimap std::unordered_multimap
#define uset std::unordered_set
#define umultiset std::unordered_multiset

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
#elif defined(TR1_UNORDERED)
#include <functional>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <utility>

#define umap std::tr1::unordered_map
#define umultimap std::tr1::unordered_multimap
#define uset std::tr1::unordered_set
#define umultiset std::tr1::unordered_multiset

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
template <class Key, class T, class Hash, class Pred, class Alloc>
bool operator==(umap<Key, T, Hash, Pred, Alloc> const& lhs,
                umap<Key, T, Hash, Pred, Alloc> const& rhs) {
  if (lhs.size() != rhs.size())
    return (false);
  for (typename umap<Key, T, Hash, Pred, Alloc>::const_iterator it(lhs.begin()),
       end(lhs.end());
       it != end;
       ++it) {
    typename umap<Key, T, Hash, Pred, Alloc>::const_iterator it_find(
        rhs.find(it->first));
    if (it_find == rhs.end() || it_find->second != it->second)
      return (false);
  }
  return (true);
}
// Missing not equal operator for unrodered map on tr1.
template <class Key, class T, class Hash, class Pred, class Alloc>
bool operator!=(umap<Key, T, Hash, Pred, Alloc> const& lhs,
                umap<Key, T, Hash, Pred, Alloc> const& rhs) {
  return (!operator==(lhs, rhs));
}

// Missing equal operator for unrodered multimap on tr1.
template <class Key, class T, class Hash, class Pred, class Alloc>
bool operator==(umultimap<Key, T, Hash, Pred, Alloc> const& lhs,
                umultimap<Key, T, Hash, Pred, Alloc> const& rhs) {
  if (lhs.size() != rhs.size())
    return (false);
  for (typename umap<Key, T, Hash, Pred, Alloc>::const_iterator it(lhs.begin()),
       end(lhs.end());
       it != end;
       ++it) {
    bool find(false);
    for (typename umap<Key, T, Hash, Pred, Alloc>::const_iterator
             it_find(rhs.find(it->first)),
         end(rhs.end());
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
template <class Key, class T, class Hash, class Pred, class Alloc>
bool operator!=(umultimap<Key, T, Hash, Pred, Alloc> const& lhs,
                umultimap<Key, T, Hash, Pred, Alloc> const& rhs) {
  return (!operator==(lhs, rhs));
}
}

// Used std implementation.
#else
#include <map>
#include <set>
#define umap std::map
#define umultimap std::multimap
#define uset std::set
#define umultiset std::multiset
#endif  // C++0X, tr1 or std

#endif  // !CC_UNORDERED_HASH_HH
