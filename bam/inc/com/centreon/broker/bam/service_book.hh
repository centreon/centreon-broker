/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_SERVICE_BOOK_HH
#define CCB_BAM_SERVICE_BOOK_HH

#include <map>
#include <memory>

#include "com/centreon/broker/bam/service_listener.hh"
#include "com/centreon/broker/io/stream.hh"

CCB_BEGIN()

// Forward declarations.
namespace neb {
class acknowledgement;
class downtime;
class service_status;
}  // namespace neb

namespace bam {
// Forward declarations.
class service_listener;

/**
 *  @class service_book service_book.hh
 * "com/centreon/broker/bam/service_book.hh"
 *  @brief Propagate service updates.
 *
 *  Propagate updates of services to service listeners.
 */
class service_book {
  typedef std::multimap<std::pair<uint32_t, uint32_t>, service_listener*>
      multimap;

  multimap _book;

 public:
  service_book() = default;
  ~service_book() noexcept = default;
  service_book(const service_book&) = delete;
  service_book& operator=(const service_book&) = delete;
  void listen(uint32_t host_id, uint32_t service_id, service_listener* listnr);
  void unlisten(uint32_t host_id,
                uint32_t service_id,
                service_listener* listnr);
  /**
   * @brief This method propagates events of type T to the concerned services
   * and then to the corresponding kpi.
   *
   * @tparam T A neb::downtime, neb::service_status, neb::acknowledgement
   * @param t The event to handle.
   * @param visitor The stream to write into.
   */
  template <typename T>
  void update(const std::shared_ptr<T>& t, io::stream* visitor = nullptr) {
    std::pair<multimap::iterator, multimap::iterator> range{
        _book.equal_range(std::make_pair(t->host_id, t->service_id))};
    while (range.first != range.second) {
      range.first->second->service_update(t, visitor);
      ++range.first;
    }
  }
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_SERVICE_BOOK_HH
