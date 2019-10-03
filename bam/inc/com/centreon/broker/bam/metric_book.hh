/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_METRIC_BOOK_HH
#define CCB_BAM_METRIC_BOOK_HH

#include <map>
#include <memory>
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace storage {
class metric;
}

namespace bam {
// Forward declarations.
class metric_listener;

/**
 *  @class metric_book metric_book.hh "com/centreon/broker/bam/metric_book.hh"
 *  @brief Propagate metric updates.
 *
 *  Propagate updates of metrics to metric listeners.
 */
class metric_book {
 public:
  metric_book();
  metric_book(metric_book const& other);
  ~metric_book();
  metric_book& operator=(metric_book const& other);
  void listen(uint32_t metric_id, metric_listener* listnr);
  void unlisten(uint32_t metric_id, metric_listener* listnr);
  void update(std::shared_ptr<storage::metric> const& m,
              io::stream* visitor = NULL);

 private:
  typedef std::multimap<uint32_t, metric_listener*> multimap;

  multimap _book;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_METRIC_BOOK_HH
