/*
** Copyright 2015-2021 Centreon
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

#include "com/centreon/broker/processing/acceptor.hh"

#include <unistd.h>

#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/processing/feeder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**
 *  Constructor.
 *
 *  @param[in] endp       Endpoint.
 *  @param[in] name       Name of the endpoint.
 */
acceptor::acceptor(std::shared_ptr<io::endpoint> endp, std::string const& name)
    : endpoint(true, name),
      _state(stopped),
      _should_exit(false),
      _endp(endp),
      _retry_interval(30) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  exit();
}

/**
 *  Accept a new incoming connection.
 */
void acceptor::accept() {
  static uint32_t connection_id = 0;

  // Try to accept connection.
  std::unique_ptr<io::stream> u = _endp->open();

  if (u) {
    // Create feeder thread.
    std::string name(fmt::format("{}-{}", _name, ++connection_id));
    log_v2::core()->info("New incoming connection '{}'", name);
    std::unique_ptr<processing::feeder> f(
        new processing::feeder(name, u, _read_filters, _write_filters));

    std::lock_guard<std::mutex> lock(_stat_mutex);
    _feeders.emplace_back(f.release());
    log_v2::core()->trace("Currently {} connections to acceptor '{}'",
                          _feeders.size(), _name);
  } else
    log_v2::core()->debug("accept ('{}') failed.", _name);
}

/**
 *  Exit this thread.
 */
void acceptor::exit() {
  std::unique_lock<std::mutex> lck(_state_m);
  switch (_state) {
    case stopped:
      _state = finished;
      break;
    case running:
      _should_exit = true;
      _state_cv.wait(lck, [this] { return _state == acceptor::finished; });
      _thread.join();
      break;
    case finished:
      break;
  }

  for (auto it = _feeders.begin(); it != _feeders.end(); ++it) {
    delete *it;
    *it = nullptr;
  }
}

/**
 *  @brief Set read filters.
 *
 *  This is only useful in input mode.
 *
 *  @param[in] filters  Set of accepted event IDs.
 */
void acceptor::set_read_filters(std::unordered_set<uint32_t> const& filters) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  _read_filters = filters;
  _read_filters_str = misc::dump_filters(_read_filters);
}

/**
 *  @brief Set retry interval of the acceptor.
 *
 *  The retry interval is only used in case of error of the acceptor. In
 *  normal operation mode, connections are accepted as soon as possible.
 *
 *  @param[in] retry_interval  Retry interval between two client
 *                             acception attempts in case of error at
 *                             the first attempt.
 */
void acceptor::set_retry_interval(time_t retry_interval) {
  _retry_interval = retry_interval;
}

/**
 *  @brief Set write filters.
 *
 *  This is useful to prevent endpoints of generating some kind of
 *  events.
 */
void acceptor::set_write_filters(std::unordered_set<uint32_t> const& filters) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  _write_filters = filters;
  _write_filters_str = misc::dump_filters(_write_filters);
}

/**
 *  Get the read filters used by the feeder.
 *
 *  @return  The read filters used by the feeder.
 */
std::string const& acceptor::_get_read_filters() const {
  return _read_filters_str;
}

/**
 *  Get the write filters used by the feeder.
 *
 *  @return  The write filters used by the feeder.
 */
std::string const& acceptor::_get_write_filters() const {
  return _write_filters_str;
}

/**
 *  Forward the statistic to the feeders.
 *
 *  @param[in] tree  The tree.
 */
void acceptor::_forward_statistic(nlohmann::json& tree) {
  // Get statistic of acceptor.
  _endp->stats(tree);
  // Get statistics of feeders
  for (auto it = _feeders.begin(), end = _feeders.end(); it != end; ++it) {
    nlohmann::json subtree;
    (*it)->stats(subtree);
    tree[(*it)->get_name()] = std::move(subtree);
  }
}

void acceptor::_set_listening(bool listening) noexcept {
  _listening = listening;
  set_state(listening ? "listening" : "disconnected");
}

uint32_t acceptor::_get_queued_events() const {
  return 0;
}

/**
 *  Start bthread.
 */
void acceptor::start() {
  std::unique_lock<std::mutex> lock(_state_m);
  if (_state == stopped) {
    _should_exit = false;
    _thread = std::thread(&acceptor::_callback, this);
    _state_cv.wait(lock, [this] { return _state == acceptor::running; });
  }
}

void acceptor::_callback() noexcept {
  std::unique_lock<std::mutex> lock(_state_m);
  _state = running;
  _state_cv.notify_all();
  lock.unlock();

  // Run as long as no exit request was made.
  while (!_should_exit) {
    try {
      _set_listening(true);
      // Try to accept connection.
      accept();
    } catch (std::exception const& e) {
      _set_listening(false);
      // Log error.
      log_v2::core()->error(
          "acceptor: endpoint '{}' could not accept client: {}", _name,
          e.what());

      // Sleep a while before reconnection.
      log_v2::core()->debug(
          "acceptor: endpoint '{}' will wait {}s before attempting to accept a "
          "new client",
          _name, _retry_interval);
      time_t limit{time(nullptr) + _retry_interval};
      while (!_endp->is_ready() && !_should_exit && time(nullptr) < limit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }

    // Check for terminated feeders.
    {
      std::lock_guard<std::mutex> lock(_stat_mutex);
      for (auto it = _feeders.begin(), end = _feeders.end(); it != end;) {
        log_v2::core()->trace("acceptor '{}' feeder '{}' state {}", _name,
                              (*it)->get_name(), (*it)->get_state());
        if ((*it)->is_finished()) {
          log_v2::core()->info("removing '{}' from acceptor '{}'",
                               (*it)->get_name(), _name);
          it = _feeders.erase(it);
        } else
          ++it;
      }
    }
  }
  log_v2::core()->info("processing acceptor '{}' finished", _name);
  _set_listening(false);

  lock.lock();
  _state = acceptor::finished;
  _state_cv.notify_all();
}
