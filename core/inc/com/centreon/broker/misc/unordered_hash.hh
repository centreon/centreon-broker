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

#ifndef CCB_MISC_UNORDERED_HASH_HH
#  define CCB_MISC_UNORDERED_HASH_HH

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
#    include <unordered_map>
#    include <unordered_set>
#    define umap std::unordered_map
#    define umultimap std::unordered_multimap
#    define uset std::unordered_set
#    define umultiset std::unordered_multiset
// Used tr1 implementation.
#  elif defined(TR1_UNORDERED)
#    include <tr1/unordered_map>
#    include <tr1/unordered_set>
#    define umap std::tr1::unordered_map
#    define umultimap std::tr1::unordered_multimap
#    define uset std::tr1::unordered_set
#    define umultiset std::tr1::unordered_multiset
// Used std implementation.
#  else
#    include <map>
#    include <set>
#    define umap std::map
#    define umultimap std::multimap
#    define uset std::set
#    define umultiset std::multiset
#  endif // C++0X, tr1 or std

#endif // !CCB_MISC_UNORDERED_HASH_HH
