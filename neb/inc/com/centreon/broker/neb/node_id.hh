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

#ifndef CCB_NEB_NODE_ID_HH
#define CCB_NEB_NODE_ID_HH

#include <cstdint>
#include <functional>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class node_id node_id.hh "com/centreon/broker/neb/node_id.hh"
 *  @brief Node id object.
 *
 *  This object contains the id of a node: a host id and an associated
 *  service id.  Both the host id or the service id can be zero for service
 *  associated with no hosts and inversely.
 */
class node_id {
 public:
  node_id();
  node_id(node_id const& obj);
  node_id& operator=(node_id const& obj);
  bool operator==(node_id const& other) const throw();
  explicit node_id(uint64_t host_id, uint64_t service_id = 0);
  bool operator<(node_id const& obj) const throw();
  bool operator!=(node_id const& obj) const throw();

  uint64_t get_host_id() const throw();
  uint64_t get_service_id() const throw();
  bool is_host() const throw();
  bool is_service() const throw();
  node_id to_host() const throw();
  bool empty() const throw();

 private:
  uint64_t _host_id;
  uint64_t _service_id;
};
}  // namespace neb

CCB_END()

namespace std {
template <>
struct hash<com::centreon::broker::neb::node_id> {
  std::size_t operator()(const com::centreon::broker::neb::node_id& k) const {
    uint32_t short_random1 = 842304669U;
    uint32_t short_random2 = 619063811U;
    uint32_t short_random3 = 937041849U;
    uint32_t short_random4 = 3309708029U;
    uint32_t value1a = static_cast<uint32_t>(k.get_host_id() & 0xffffffff);
    uint32_t value1b =
        static_cast<uint32_t>((k.get_host_id() >> 32) & 0xffffffff);
    uint32_t value2a = static_cast<uint32_t>(k.get_service_id() & 0xffffffff);
    uint32_t value2b =
        static_cast<uint32_t>((k.get_service_id() >> 32) & 0xffffffff);
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

#endif  // !CCB_NEB_NODE_ID_HH
