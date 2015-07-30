/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
 *  @param[in] source_broker_id  Source Broker ID.
 *  @param[in] command_id        Command ID.
 *
 *  @return Command result.
 */
command_result command_listener::command_status(
                                   unsigned int source_broker_id,
                                   unsigned int command_id) {
  // Check for entries that should be removed from cache.
  _check_invalid();

  command_result res;
  QMutexLocker lock(&_pendingm);
  std::map<std::pair<unsigned int, unsigned int>, pending_command>::const_iterator
    it(_pending.find(std::make_pair(source_broker_id, command_id)));
  // Command result exists.
  if (it != _pending.end()) {
    res = it->second.result;
  }
  // Fake command result.
  else {
    lock.unlock();
    res.id = command_id;
    res.destination_id = source_broker_id;
    res.code = -1;
    std::ostringstream oss;
    oss << "Command " << command_id << " of Centreon Broker "
        << source_broker_id
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
unsigned int command_listener::write(
                                 misc::shared_ptr<io::data> const& d) {
  if (!d.isNull()) {
    // Command request, store it in the cache.
    if (d->type() == command_request::static_type()) {
      command_request const& req(d.ref_as<command_request const>());
      QMutexLocker lock(&_pendingm);
      std::map<std::pair<unsigned int, unsigned int>, pending_command>::iterator
        it(_pending.find(std::make_pair(req.source_id, req.id)));
      if (it == _pending.end()) {
        pending_command&
          p(_pending[std::make_pair(req.source_id, req.id)]);
        p.invalid_time = time(NULL) + _request_timeout;
        p.result.id = req.id;
        p.result.code = 1;
        p.result.msg = "Pending";
        if (p.invalid_time < _next_invalid)
          _next_invalid = p.invalid_time;
      }
    }
    // Command result, store it in the cache.
    else if (d->type() == command_result::static_type()) {
      command_result const& res(d.ref_as<command_result const>());
      QMutexLocker lock(&_pendingm);
      pending_command&
        p(_pending[std::make_pair(res.destination_id, res.id)]);
      p.result = res;
      p.invalid_time = time(NULL) + _result_timeout;
      if (p.invalid_time < _next_invalid)
        _next_invalid = p.invalid_time;
    }
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
  for (std::map<std::pair<unsigned int, unsigned int>, pending_command>::iterator
         it(_pending.begin()),
         end(_pending.end());
       it != end;) {
    if (it->second.invalid_time < now) {
      if (it->second.result.code == 1) { // Pending.
        it->second.invalid_time = now + _result_timeout;
        it->second.result.code = -1;
        it->second.result.msg = "Command timeout";
        ++it;
      }
      else {
        std::map<std::pair<unsigned int, unsigned int>, pending_command>::iterator
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
