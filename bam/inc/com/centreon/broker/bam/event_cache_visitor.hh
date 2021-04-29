/*
** Copyright 2014-2015, 2020-2021 Centreon
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

#ifndef CCB_BAM_EVENT_CACHE_VISITOR_HH
#define CCB_BAM_EVENT_CACHE_VISITOR_HH

#include <vector>

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class event_cache_visitor event_cache_visitor.hh
 * "com/centreon/broker/bam/event_cache_visitor.hh"
 *  @brief event_cache_visitor cache the events
 *
 *  This class caches the events and commit them to the multiplexing in
 *  this order: others, ba_events, kpi_events.
 */
class event_cache_visitor : public io::stream {
  std::vector<std::shared_ptr<io::data> > _others;
  std::vector<std::shared_ptr<io::data> > _ba_events;
  std::vector<std::shared_ptr<io::data> > _kpi_events;

 public:
  event_cache_visitor();
  ~event_cache_visitor() noexcept = default;
  event_cache_visitor(const event_cache_visitor&) = delete;
  event_cache_visitor& operator=(const event_cache_visitor&) = delete;
  void commit_to(io::stream& to);
  virtual bool read(std::shared_ptr<io::data>& d, time_t deadline) override;
  virtual int write(std::shared_ptr<io::data> const& d) override;
  int32_t stop() override { return 0; }
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_EVENT_CACHE_VISITOR_HH
