/*
** Copyright 2011-2013,2015-2017 Centreon
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

#ifndef CCB_PROCESSING_FAILOVER_HH
#define CCB_PROCESSING_FAILOVER_HH

#include <climits>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/processing/acceptor.hh"
#include "com/centreon/broker/processing/endpoint.hh"

CCB_BEGIN()

// Forward declarations.
namespace io {
class properties;
}
namespace stats {
class builder;
}

namespace processing {
/**
 *  @class failover failover.hh "com/centreon/broker/processing/failover.hh"
 *  @brief Failover thread.
 *
 *  Thread that provide failover on output endpoints.
 *
 *  Multiple failover can be forwarded.
 */
class failover : public endpoint {
  friend class stats::builder;
  std::atomic_bool _should_exit;

  std::thread _thread;
  bool _started;
  mutable std::mutex _started_m;
  std::condition_variable _started_cv;
  bool _stopped;
  mutable std::mutex _stopped_m;
  std::condition_variable _stopped_cv;
  void _run();

 public:
  failover(std::shared_ptr<io::endpoint> endp,
           std::shared_ptr<multiplexing::subscriber> sbscrbr,
           std::string const& name);
  failover(failover const& other) = delete;
  failover& operator=(failover const& other) = delete;
  ~failover();
  void add_secondary_endpoint(std::shared_ptr<io::endpoint> endp);
  time_t get_buffering_timeout() const throw();
  bool get_initialized() const throw();
  time_t get_retry_interval() const throw();
  void start() override;
  void exit() override final;
  bool should_exit() const;
  void set_buffering_timeout(time_t secs);
  void set_failover(std::shared_ptr<processing::failover> fo);
  void set_retry_interval(time_t retry_interval);
  void update() override;

 protected:
  // From stat_visitable
  std::string const& _get_read_filters() const override;
  std::string const& _get_write_filters() const override;
  uint32_t _get_queued_events() const override;
  virtual void _forward_statistic(json11::Json::object& tree) override;

 private:
  void _launch_failover();
  void _update_status(std::string const& status);

  // Data that doesn't require locking.
  volatile time_t _buffering_timeout;
  std::shared_ptr<io::endpoint> _endpoint;
  std::vector<std::shared_ptr<io::endpoint> > _secondary_endpoints;
  std::shared_ptr<failover> _failover;
  bool _failover_launched;
  volatile bool _initialized;
  time_t _next_timeout;
  volatile time_t _retry_interval;
  std::shared_ptr<multiplexing::subscriber> _subscriber;
  volatile bool _update;

  // Status.
  std::string _status;
  mutable std::mutex _status_m;

  // Stream.
  std::shared_ptr<io::stream> _stream;
  mutable std::timed_mutex _stream_m;
};
}  // namespace processing

CCB_END()

#endif  // !CCB_PROCESSING_FAILOVER_HH
