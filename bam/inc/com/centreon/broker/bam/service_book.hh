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

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

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
  using multimap =
      std::multimap<std::pair<uint32_t, uint32_t>, service_listener*>;
  multimap _book;

 public:
  service_book() = default;
  service_book(const service_book& other);
  ~service_book() noexcept = default;
  service_book& operator=(const service_book& other);
  void listen(uint32_t host_id, uint32_t service_id, service_listener* listnr);
  void unlisten(uint32_t host_id,
                uint32_t service_id,
                service_listener* listnr);
  void update(const std::shared_ptr<neb::service_status>& ss,
              io::stream* visitor = nullptr);
  void update(const std::shared_ptr<neb::acknowledgement>& ack,
              io::stream* visitor = nullptr);
  void update(const std::shared_ptr<neb::downtime>& dt,
              io::stream* visitor = nullptr);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_SERVICE_BOOK_HH
