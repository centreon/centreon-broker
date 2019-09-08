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
 public:
  service_book();
  service_book(service_book const& other);
  ~service_book();
  service_book& operator=(service_book const& other);
  void listen(unsigned int host_id,
              unsigned int service_id,
              service_listener* listnr);
  void unlisten(unsigned int host_id,
                unsigned int service_id,
                service_listener* listnr);
  void update(std::shared_ptr<neb::service_status> const& ss,
              io::stream* visitor = NULL);
  void update(std::shared_ptr<neb::acknowledgement> const& ack,
              io::stream* visitor = NULL);
  void update(std::shared_ptr<neb::downtime> const& dt,
              io::stream* visitor = NULL);

 private:
  typedef std::multimap<std::pair<unsigned int, unsigned int>,
                        service_listener*>
      multimap;

  multimap _book;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_SERVICE_BOOK_HH
