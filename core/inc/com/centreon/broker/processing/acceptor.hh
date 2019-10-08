/*
** Copyright 2015-2019 Centreon
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

#ifndef CCB_PROCESSING_ACCEPTOR_HH
#define CCB_PROCESSING_ACCEPTOR_HH

#include <ctime>
#include <list>
#include <memory>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

// Forward declaration.
namespace io {
class endpoint;
}

namespace processing {
// Forward declaration.
class feeder;

/**
 *  @class acceptor acceptor.hh "com/centreon/broker/processing/acceptor.hh"
 *  @brief Accept incoming connections.
 *
 *  Accept incoming connections and launch a feeder thread.
 */
class acceptor : public bthread {
 public:
  acceptor(std::shared_ptr<io::endpoint> endp, std::string const& name);
  ~acceptor();
  void accept();
  void exit() override;
  void run() override;
  void set_read_filters(std::unordered_set<uint32_t> const& filters);
  void set_retry_interval(time_t retry_interval);
  void set_write_filters(std::unordered_set<uint32_t> const& filters);

 protected:
  // From stat_visitable
  char const* _get_state() const override;
  virtual uint32_t _get_queued_events() override;
  std::unordered_set<uint32_t> const& _get_read_filters() const override;
  std::unordered_set<uint32_t> const& _get_write_filters() const override;
  virtual void _forward_statistic(json11::Json::object& tree) override;

 private:
  acceptor(acceptor const& other);
  acceptor& operator=(acceptor const& other);
  void _wait_feeders();
  void _set_listening(bool val);
  bool _get_listening() const throw();

  std::shared_ptr<io::endpoint> _endp;
  std::list<std::shared_ptr<processing::feeder> > _feeders;
  std::unordered_set<uint32_t> _read_filters;
  time_t _retry_interval;
  std::unordered_set<uint32_t> _write_filters;
  bool _listening;
};
}  // namespace processing

CCB_END()

#endif  // !CCB_PROCESSING_ACCEPTOR_HH
