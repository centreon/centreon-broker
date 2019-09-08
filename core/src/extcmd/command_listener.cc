/*
** Copyright 2015-2017 Centreon
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

#include "com/centreon/broker/extcmd/command_listener.hh"
#include <sstream>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**
 *  Constructor.
 */
command_listener::command_listener() : _next_invalid(0) {}

/**
 *  Destructor.
 */
command_listener::~command_listener() {}

/**
 *  Get status of command.
 *
 *  @param[in] command_uuid      Command UUID.
 *
 *  @return Command result.
 */
command_result command_listener::command_status(
    std::string const& command_uuid) {
  // Check for entries that should be removed from cache.
  _check_invalid();

  command_result res;
  std::unique_lock<std::mutex> lock(_pendingm);
  std::map<std::string, pending_command>::iterator it(
      _pending.find(command_uuid));
  // Command result exists.
  if (it != _pending.end())
    _extract_command_result(res, it->second);
  // Fake command result.
  else {
    lock.unlock();
    res.uuid = command_uuid;
    res.destination_id = io::data::broker_id;
    res.code = -1;
    std::ostringstream oss;
    oss << "\"Command " << command_uuid
        << " is not available (invalid command ID, timeout, ?)\"";
    res.msg = oss.str();
  }
  return res;
}

/**
 *  Read from command listener.
 *
 *  @param[in] d         Unused.
 *  @param[in] deadline  Unused.
 *
 *  @return This method will throw.
 */
bool command_listener::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw(exceptions::shutdown() << "cannot read from command listener");
  return (true);
}

/**
 *  Write to command listener.
 *
 *  @param[in] d  Command listener only process command requests and
 *                command results.
 */
int command_listener::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, "command"))
    return 1;

  // Command request, store it in the cache.
  if (d->type() == command_request::static_type()) {
    command_request const& req(
        *std::static_pointer_cast<command_request const>(d));
    std::lock_guard<std::mutex> lock(_pendingm);
    std::map<std::string, pending_command>::iterator it(
        _pending.find(req.uuid));
    if (it == _pending.end()) {
      pending_command& p(_pending[req.uuid]);
      p.invalid_time = time(nullptr) + _request_timeout;
      p.uuid = req.uuid;
      p.code = 1;
      p.with_partial_result = req.with_partial_result;
      if (p.invalid_time < _next_invalid)
        _next_invalid = p.invalid_time;
    }
  }
  // Command result, store it in the cache.
  else if (d->type() == command_result::static_type()) {
    command_result const& res(
        *std::static_pointer_cast<command_result const>(d));
    std::lock_guard<std::mutex> lock(_pendingm);
    pending_command& p(_pending[res.uuid]);
    p.code = res.code;
    p.msgs.push_back(res.msg);
    p.invalid_time = time(nullptr) + _result_timeout;
    if (p.invalid_time < _next_invalid)
      _next_invalid = p.invalid_time;
  }

  // Check for entries that should be removed from cache.
  _check_invalid();

  return (1);
}

/**
 *  Check for invalid entries in cache.
 */
void command_listener::_check_invalid() {
  time_t now(time(nullptr));
  _next_invalid = now + 24 * 60 * 60;
  std::lock_guard<std::mutex> lock(_pendingm);
  for (std::map<std::string, pending_command>::iterator it(_pending.begin()),
       end(_pending.end());
       it != end;) {
    if (it->second.invalid_time < now) {
      if (it->second.code == 1) {  // Pending.
        it->second.invalid_time = now + _result_timeout;
        it->second.code = -1;
        it->second.msgs.clear();
        it->second.msgs.push_back("\"Command timeout\"");
        ++it;
      } else {
        std::map<std::string, pending_command>::iterator to_delete(it);
        ++it;
        _pending.erase(to_delete);
      }
    } else if (it->second.invalid_time < _next_invalid) {
      _next_invalid = it->second.invalid_time;
      ++it;
    } else
      ++it;
  }
  return;
}

/**
 *  Extract next command result.
 *
 *  @param[out]    res      Command result.
 *  @param[in,out] pending  Pending command.
 */
void command_listener::_extract_command_result(command_result& res,
                                               pending_command& pending) {
  // Set basic properties.
  res.code = pending.code;
  res.uuid = pending.uuid;

  if (!pending.msgs.empty()) {
    // Merge results if necessary.
    if (!pending.with_partial_result && (pending.msgs.size() != 1)) {
      std::string msg;
      for (std::list<std::string>::const_iterator it(pending.msgs.begin()),
           end(pending.msgs.end());
           it != end; ++it) {
        msg.append(*it);
      }
      pending.msgs.clear();
      pending.msgs.push_back(msg);
    }

    // Extract next result.
    res.msg = pending.msgs.front();

    // Discard this result if partial result mode is enabled.
    if (pending.with_partial_result)
      pending.msgs.pop_front();
  }
  return;
}
