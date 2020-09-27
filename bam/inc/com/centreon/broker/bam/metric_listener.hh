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

#ifndef CCB_BAM_METRIC_LISTENER_HH
#define CCB_BAM_METRIC_LISTENER_HH

#include <memory>

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace storage {
class metric;
}

namespace bam {
/**
 *  @class metric_listener metric_listener.hh
 * "com/centreon/broker/bam/metric_listener.hh"
 *  @brief Listen to metric state change.
 *
 *  This interface is used by classes wishing to listen to metric
 *  state change.
 */
class metric_listener {
 public:
  metric_listener();
  metric_listener(metric_listener const& other);
  virtual ~metric_listener();
  metric_listener& operator=(metric_listener const& other);

  /**
   *  Notify of metric update.
   *
   *  @param[in] status Metric status.
   */
  virtual void metric_update(std::shared_ptr<storage::metric> const& m,
                             io::stream* visitor = NULL) = 0;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_METRIC_LISTENER_HH
