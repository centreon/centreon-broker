/*
** Copyright 2015 Centreon
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

#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"

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
                                   QString const& command_uuid) {
  // Check for entries that should be removed from cache.
  _check_invalid();

  command_result res;
  QMutexLocker lock(&_pendingm);
  std::map<std::string, pending_command>::iterator
    it(_pending.find(command_uuid.toStdString()));
  // Command result exists.
  if (it != _pending.end()) {
    res = it->second.result;
    if (it->second.with_partial_result)
      it->second.result.msg.clear();
  }
  // Fake command result.
  else {
    lock.unlock();
    res.uuid = command_uuid;
    res.destination_id = io::data::broker_id;
    res.code = -1;
    std::ostringstream oss;
    oss << "Command " << command_uuid.toStdString()
        << " is not available (invalid command ID, timeout, ?)";
    res.msg = oss.str().c_str();
  }
  return (res);
}

/**
 *  Read from command listener.
 *
 *  @param[in] d         Unused.
 *  @param[in] deadline  Unused.
 *
 *  @return This method will throw.
 */
bool command_listener::read(
                         misc::shared_ptr<io::data>& d,
                         time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from command listener");
  return (true);
}

/**
 *  Write to command listener.
 *
 *  @param[in] d  Command listener only process command requests and
 *                command results.
 */
int command_listener::write(misc::shared_ptr<io::data> const& d) {
  if (!validate(d, "command"))
    return (1);

  // Command request, store it in the cache.
  if (d->type() == command_request::static_type()) {
    command_request const& req(d.ref_as<command_request const>());
    QMutexLocker lock(&_pendingm);
    std::map<std::string, pending_command>::iterator
      it(_pending.find(req.uuid.toStdString()));
    if (it == _pending.end()) {
      pending_command&
        p(_pending[req.uuid.toStdString()]);
      p.invalid_time = time(NULL) + _request_timeout;
      p.result.uuid = req.uuid;
      p.result.code = 1;
      p.result.msg = "\"Pending\"";
      p.with_partial_result = req.with_partial_result;
      if (p.invalid_time < _next_invalid)
        _next_invalid = p.invalid_time;
    }
  }
  // Command result, store it in the cache.
  else if (d->type() == command_result::static_type()) {
    command_result const& res(d.ref_as<command_result const>());
    QMutexLocker lock(&_pendingm);
    pending_command&
      p(_pending[res.uuid.toStdString()]);
    if (p.with_partial_result == false)
      p.result = res;
    else
      _merge_partial_result(p, res);
    p.invalid_time = time(NULL) + _result_timeout;
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
  time_t now(time(NULL));
  _next_invalid = now + 24 * 60 * 60;
  QMutexLocker lock(&_pendingm);
  for (std::map<std::string, pending_command>::iterator
         it(_pending.begin()),
         end(_pending.end());
       it != end;) {
    if (it->second.invalid_time < now) {
      if (it->second.result.code == 1) { // Pending.
        it->second.invalid_time = now + _result_timeout;
        it->second.result.code = -1;
        it->second.result.msg = "\"Command timeout\"";
        ++it;
      }
      else {
        std::map<std::string, pending_command>::iterator
          to_delete(it);
        ++it;
        _pending.erase(to_delete);
      }
    }
    else if (it->second.invalid_time < _next_invalid) {
      _next_invalid = it->second.invalid_time;
      ++it;
    }
    else
      ++it;
  }
  return ;
}

/**
 *  Merge partial result.
 *
 *  @param[out] dest  The destination of the merge.
 *  @param[in] res    The partial result to merge.
 */
void command_listener::_merge_partial_result(
                         pending_command& dest,
                         command_result const& res) {
  dest.result.msg.append(res.msg);
}
