/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_NEB_STRING_PAIR_HH
#  define CCB_NEB_STRING_PAIR_HH

#include <functional>

namespace std {
template <>
struct hash<std::pair<std::string, std::string>> {
  size_t operator()(std::pair<std::string, std::string> const& kt) const {
    size_t hash = 0;
    hash_combine(hash, kt.first);
    hash_combine(hash, kt.second);
    return hash;
  }
  // taken from boost::hash_combine:
  // https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
  template <class T>
  inline static void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};

template <>
struct hash<std::pair<uint64_t, uint64_t>> {
  std::size_t operator()(std::pair<uint64_t, uint64_t> const& k) const {
    uint32_t short_random1 = 842304669U;
    uint32_t short_random2 = 619063811U;
    uint32_t short_random3 = 937041849U;
    uint32_t short_random4 = 3309708029U;
    uint32_t value1a = static_cast<uint32_t>(k.first & 0xffffffff);
    uint32_t value1b =
        static_cast<uint32_t>((k.first >> 32) & 0xffffffff);
    uint32_t value2a = static_cast<uint32_t>(k.second & 0xffffffff);
    uint32_t value2b =
        static_cast<uint32_t>((k.second >> 32) & 0xffffffff);
    uint64_t product1 = static_cast<uint64_t>(value1a) * short_random1;
    uint64_t product2 = static_cast<uint64_t>(value1b) * short_random2;
    uint64_t product3 = static_cast<uint64_t>(value2a) * short_random3;
    uint64_t product4 = static_cast<uint64_t>(value2b) * short_random4;
    uint64_t hash64 = product1 + product2 + product3 + product4;
    if (sizeof(size_t) >= sizeof(uint64_t))
      return static_cast<size_t>(hash64);
    uint64_t odd_random = 1578233944LL << 32 | 194370989LL;
    uint32_t shift_random = 20591U << 16;
    hash64 = hash64 * odd_random + shift_random;
    size_t high_bits = static_cast<size_t>(
        hash64 >> (8 * (sizeof(uint64_t) - sizeof(size_t))));
    return high_bits;
  }
};
}  // namespace std

#endif /* CCB_NEB_STRING_PAIR_HH */
