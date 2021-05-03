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

#include "com/centreon/broker/bam/metric_book.hh"

#include "com/centreon/broker/bam/metric_listener.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker::bam;

/**
 *  Make a metric listener listen to metric updates.
 *
 *  @param[in]     metric_id  Metric ID.
 *  @param[in,out] listnr     Metric listener.
 */
void metric_book::listen(uint32_t metric_id, metric_listener* listnr) {
  _book.insert(std::make_pair(metric_id, listnr));
}

/**
 *  Remove a listener.
 *
 *  @param[in] metric_id  Metric ID.
 *  @param[in] listnr      Metric listener.
 */
void metric_book::unlisten(uint32_t metric_id, metric_listener* listnr) {
  std::pair<multimap::iterator, multimap::iterator> range(
      _book.equal_range(metric_id));
  for (; range.first != range.second; ++range.first)
    if (range.first->second == listnr) {
      _book.erase(range.first);
      break;
    }
}

/**
 *  Update all metric listeners related to the metric.
 *
 *  @param[in]  m        Metric status.
 *  @param[out] visitor  Object that will receive events.
 */
void metric_book::update(std::shared_ptr<storage::metric> const& m,
                         io::stream* visitor) {
  std::pair<multimap::iterator, multimap::iterator> range(
      _book.equal_range(m->metric_id));
  while (range.first != range.second) {
    range.first->second->metric_update(m, visitor);
    ++range.first;
  }
}
