/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_PROCESSING_FEEDER_HH
#define CCB_PROCESSING_FEEDER_HH

#include <memory>
#include <string>
#include "com/centreon/broker/misc/shared_mutex.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

// Forward declaration.
namespace io {
class stream;
}

namespace processing {
/**
 *  @class feeder feeder.hh "com/centreon/broker/processing/feeder.hh"
 *  @brief Feed events from a source to a destination.
 *
 *  Take events from a source and send them to a destination.
 */
class feeder : public bthread {
 public:
  feeder(std::string const& name,
         std::shared_ptr<io::stream> client,
         std::unordered_set<uint32_t> const& read_filters,
         std::unordered_set<uint32_t> const& write_filters);
  ~feeder();
  void run();

 protected:
  // From stat_visitable
  virtual std::string _get_state();
  virtual uint32_t _get_queued_events();
  std::unordered_set<uint32_t> const& _get_read_filters() const override;
  std::unordered_set<uint32_t> const& _get_write_filters() const override;
  void _forward_statistic(io::properties& tree);

 private:
  feeder(feeder const& other);
  feeder& operator=(feeder const& other);

  std::shared_ptr<io::stream> _client;
  multiplexing::subscriber _subscriber;
  // This mutex is used for the stat thread.
  misc::shared_mutex _client_mutex;
};
}  // namespace processing

CCB_END()

#endif  // !CCB_PROCESSING_FEEDER_HH
