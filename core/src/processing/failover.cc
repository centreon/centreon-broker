/*
** Copyright 2011-2017 Centreon
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

#include "com/centreon/broker/processing/failover.hh"

#include <unistd.h>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] endp      Failover thread endpoint.
 *  @param[in] sbscrbr   Multiplexing agent.
 *  @param[in] name      The failover name.
 */
failover::failover(std::shared_ptr<io::endpoint> endp,
                   std::shared_ptr<multiplexing::subscriber> sbscrbr,
                   std::string const& name)
    : endpoint(name),
      _should_exit(false),
      _started(false),
      _stopped{false},
      _buffering_timeout(0),
      _endpoint(endp),
      _failover_launched(false),
      _initialized(false),
      _next_timeout(0),
      _retry_interval(30),
      _subscriber(sbscrbr),
      _update(false) {
  log_v2::core()->trace("failover '{}' construction.", _name);
}

/**
 *  Destructor.
 */
failover::~failover() {
  exit();
}

/**
 *  Add secondary endpoint to this failover thread.
 *
 *  @param[in] endp  New secondary endpoint.
 */
void failover::add_secondary_endpoint(std::shared_ptr<io::endpoint> endp) {
  _secondary_endpoints.push_back(endp);
}

/**
 *  Exit failover thread.
 */
void failover::exit() {
  log_v2::core()->trace("failover '{}' exit.", _name);
  std::unique_lock<std::mutex> lock(_stopped_m);
  if (_started) {
    if (!_should_exit) {
      _should_exit = true;
      log_v2::config()->trace("Waiting for {} to be stopped", _name);
      _stopped_cv.wait(lock, [this] { return _stopped; });
      _thread.join();
      _started = false;
    }
  }
  _subscriber->get_muxer().wake();
  log_v2::core()->trace("failover '{}' exited.", _name);
}

/**
 *  Get buffering timeout.
 *
 *  @return Failover thread buffering timeout.
 */
time_t failover::get_buffering_timeout() const throw() {
  return _buffering_timeout;
}

/**
 *  Check whether or not the thread has initialize.
 *
 *  @return True if the thread is initializable. That is it is read()able.
 */
bool failover::get_initialized() const throw() {
  return _initialized;
}

/**
 *  Get retry interval.
 *
 *  @return Failover thread retry interval.
 */
time_t failover::get_retry_interval() const throw() {
  return _retry_interval;
}

/**
 *  Thread core function.
 */
void failover::_run() {
  std::unique_lock<std::mutex> lock_start(_started_m);
  _started = true;
  _started_cv.notify_all();
  lock_start.unlock();
  // Initial log.
  log_v2::processing()->debug("failover: thread of endpoint '{}' is starting",
                              _name);

  // Check endpoint.
  if (!_endpoint) {
    logging::error(logging::high)
        << "failover: thread of endpoint '" << _name << "' has no endpoint"
        << " object, this is likely a software bug that should be reported"
        << " to Centreon Broker developers";
    std::unique_lock<std::mutex> lock_stop(_stopped_m);
    _stopped = true;
    _stopped_cv.notify_all();
    return;
  }

  // Thread should be aware of external exit requests.
  do {
    // This try/catch block handles any error of the current thread
    // objects. In case of an exception, it is responsible to launch
    // failovers of this failover.
    try {
      // Attempt to open endpoint.
      _update_status("opening endpoint");
      set_last_connection_attempt(timestamp::now());
      {
        std::shared_ptr<io::stream> s(_endpoint->open());
        if (!s)
          throw exceptions::msg()
              << "failover: '" << _name << "' cannot connect endpoint.";
        {
          std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
          _stream = s;
          set_state(s ? "connected" : "connecting");
        }
        _initialized = true;
        set_last_connection_success(timestamp::now());
      }
      _update_status("");
      _update = true;

      // Buffering.
      if (_buffering_timeout > 0) {
        // Status.
        log_v2::processing()->debug(
            "failover: buffering data for endpoint '{}' ({}s)", _name,
            _buffering_timeout);
        _update_status("buffering data");

        // Wait loop.
        // FIXME SGA: condvar should be more elegant...
        for (ssize_t i = 0; !should_exit() && i < _buffering_timeout * 10;
             i++) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        _update_status("");
      }

      // Open secondaries.
      _update_status("initializing secondaries");
      std::vector<std::shared_ptr<io::stream> > secondaries;
      for (std::vector<std::shared_ptr<io::endpoint> >::iterator
               it(_secondary_endpoints.begin()),
           end(_secondary_endpoints.end());
           it != end; ++it)
        try {
          std::shared_ptr<io::stream> s((*it)->open());
          if (s)
            secondaries.push_back(s);
          else
            logging::error(logging::medium)
                << "failover: could not open a secondary of endpoint '" << _name
                << ": secondary returned a null stream";
        } catch (std::exception const& e) {
          logging::error(logging::medium)
              << "failover: error occured while opening a secondary "
              << "of endpoint '" << _name << "': " << e.what();
        }
      _update_status("");

      // Shutdown failover.
      if (_failover_launched) {
        log_v2::processing()->debug(
            "failover: shutting down failover of endpoint '{}'", _name);
        _update_status("shutting down failover");
        _failover->exit();
        _failover_launched = false;
        _update_status("");
      }

      // Event processing loop.
      log_v2::processing()->debug(
          "failover: launching event loop of endpoint '{}'", _name);
      _subscriber->get_muxer().nack_events();
      bool stream_can_read(true);
      bool muxer_can_read(true);
      bool should_commit(false);
      std::shared_ptr<io::data> d;

      time_t fill_stats_time = time(nullptr);

      while (!should_exit()) {
        // Check for update.
        if (_update) {
          std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
          _stream->update();
          _update = false;
        }

        // Filling stats
        if (time(nullptr) >= fill_stats_time) {
          fill_stats_time += 5;
          set_queued_events(_subscriber->get_muxer().get_event_queue_size());
        }

        // Read from endpoint stream.
        d.reset();
        bool timed_out_stream(true);
        if (stream_can_read) {
          log_v2::processing()->debug(
              "failover: reading event from endpoint '{}'", _name);
          _update_status("reading event from stream");
          try {
            std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
            timed_out_stream = !_stream->read(d, 0);
          } catch (exceptions::shutdown const& e) {
            log_v2::processing()->debug(
                "failover: stream of endpoint '{}' shutdown while reading: {}",
                _name, e.what());
            stream_can_read = false;
          }
          if (d) {
            log_v2::processing()->debug(
                "failover: writing event of endpoint '{}' to multiplexing "
                "engine",
                _name);
            _update_status("writing event to multiplexing engine");
            _subscriber->get_muxer().write(d);
            tick();
            _update_status("");
            continue;  // Stream read bias.
          }
          _update_status("");
        }

        // Read from muxer stream.
        d.reset();
        bool timed_out_muxer(true);
        if (muxer_can_read) {
          log_v2::processing()->debug(
              "failover: reading event from "
              "multiplexing engine for endpoint '{}'",
              _name);
          _update_status("reading event from multiplexing engine");
          try {
            timed_out_muxer = !_subscriber->get_muxer().read(d, 0);
            should_commit = should_commit || d;
          } catch (exceptions::shutdown const& e) {
            log_v2::processing()->debug(
                "failover: muxer of endpoint '{}' "
                "shutdown while reading: {}",
                _name, e.what());
            muxer_can_read = false;
          }
          if (d) {
            log_v2::processing()->debug(
                "failover: writing event of multiplexing engine to endpoint "
                "'{}'",
                _name);
            _update_status("writing event to stream");
            int we(0);

            try {
              std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
              we = _stream->write(d);
            } catch (exceptions::shutdown const& e) {
              log_v2::processing()->debug(
                  "failover: stream of endpoint '{}' shutdown while writing: "
                  "{}",
                  _name, e.what());
              muxer_can_read = false;
            }
            _subscriber->get_muxer().ack_events(we);
            tick();
            for (std::vector<std::shared_ptr<io::stream> >::iterator
                     it(secondaries.begin()),
                 end(secondaries.end());
                 it != end;) {
              try {
                (*it)->write(d);
                ++it;
              } catch (std::exception const& e) {
                logging::error(logging::medium)
                    << "failover: error "
                    << "occurred while writing to a secondary of endpoint '"
                    << _name << "' (secondary will be removed): " << e.what();
                it = secondaries.erase(it);
              }
            }
            _update_status("");
          }
        }

        // If both timed out, sleep a while.
        d.reset();
        if (timed_out_stream && timed_out_muxer) {
          time_t now(time(nullptr));
          int we(0);
          if (should_commit) {
            should_commit = false;
            _next_timeout = now + 1;
            std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
            we = _stream->flush();
          } else if (now >= _next_timeout) {
            _next_timeout = now + 1;
            std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
            we = _stream->flush();
          }
          _subscriber->get_muxer().ack_events(we);
          ::usleep(100000);
        }
      }
    }
    // Some real error occured.
    catch (std::exception const& e) {
      log_v2::core()->error("failover: global error: {}", e.what());
      logging::error(logging::high) << e.what();
      {
        std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
        _stream.reset();
        set_state("connecting");
      }
      if (!should_exit()) {
        _launch_failover();
        _initialized = true;
      }
    } catch (...) {
      logging::error(logging::high)
          << "failover: endpoint '" << _name
          << "' encountered an unknown exception, this is likely a "
          << "software bug that should be reported to Centreon Broker "
             "developers";
      {
        std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
        _stream.reset();
        set_state("connecting");
      }
      if (!should_exit()) {
        _launch_failover();
        _initialized = true;
      }
    }

    // Clear stream.
    {
      std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
      _stream.reset();
      set_state("connecting");
    }

    // Sleep a while before attempting a reconnection.
    _update_status("sleeping before reconnection");

    for (ssize_t i = 0;
         !_endpoint->is_ready() && !should_exit() && i < _retry_interval; i++)
      std::this_thread::sleep_for(std::chrono::seconds(1));

    _update_status("");

  } while (!should_exit());

  // Clear stream.
  {
    std::lock_guard<std::timed_mutex> stream_lock(_stream_m);
    _stream.reset();
    set_state("connecting");
  }

  // Exit failover thread if necessary.
  if (_failover) {
    log_v2::processing()->info(
        "failover: requesting termination of failover of endpoint '{}'", _name);
    _failover->exit();
  }

  // Exit log.
  log_v2::processing()->debug("failover: thread of endpoint '{}' is exiting",
                              _name);

  std::unique_lock<std::mutex> lock_stop(_stopped_m);
  _stopped = true;
  _stopped_cv.notify_all();
}

/**
 *  Set buffering timeout.
 *
 *  @param[in] secs Buffering timeout in seconds.
 */
void failover::set_buffering_timeout(time_t secs) {
  _buffering_timeout = secs;
}

/**
 *  Set the thread's failover.
 *
 *  @param[in] fo Thread's failover.
 */
void failover::set_failover(std::shared_ptr<failover> fo) {
  _failover = fo;
}

/**
 *  Set the connection retry interval.
 *
 *  @param[in] retry_interval Time to wait between two connection
 *                            attempts.
 */
void failover::set_retry_interval(time_t retry_interval) {
  _retry_interval = retry_interval;
}

/**
 *  Configuration update request.
 */
void failover::update() {
  _update = true;
}

/**
 *  Wait for this thread to terminate along with other failovers.
 *
 *  @param[in] time  Maximum time to wait for thread termination.
 *
 *  @return True if thread exited.
 */
// bool failover::wait(unsigned long time) {
//  // Check that failover finished.
//  bool finished;
//  if (_failover)
//    finished = _failover->wait(time);
//  else
//    finished = true;
//
//  // If there was no failover or failover finished we
//  // can safely wait for ourselves.
//  if (finished)
//    finished = bthread::wait(time);
//  // Otherwise we're not finished yet.
//  else
//    finished = false;
//  return finished;
//}

/**
 *  Get the read filters used by the failover.
 *
 *  @return  The read filters used by the failover.
 */
std::string const& failover::_get_read_filters() const {
  return _subscriber->get_muxer().get_read_filters_str();
}

/**
 *  Get the write filters used by the failover.
 *
 *  @return  The write filters used by the failover.
 */
std::string const& failover::_get_write_filters() const {
  return _subscriber->get_muxer().get_write_filters_str();
}

/**
 *  Forward to failover.
 *
 *  @param[in] tree  The tree.
 */
void failover::_forward_statistic(json11::Json::object& tree) {
  {
    std::lock_guard<std::mutex> lock(_status_m);
    tree["status"] = _status;
  }
  {
    std::unique_lock<std::timed_mutex> stream_lock(_stream_m, std::defer_lock);
    if (stream_lock.try_lock_for(std::chrono::milliseconds(100))) {
      if (_stream)
        _stream->statistics(tree);
    } else
      tree["status"] = "busy";
  }
  _subscriber->get_muxer().statistics(tree);
  json11::Json::object subtree;
  if (_failover)
    _failover->stats(subtree);
  tree["failover"] = subtree;
}

/**
 *  Launch failover of this endpoint.
 */
void failover::_launch_failover() {
  _subscriber->get_muxer().nack_events();
  if (_failover && !_failover_launched) {
    _failover_launched = true;
    _failover->start();
    // FIXME DBR: what's this...
    //    while (!_failover->get_initialized() && !_failover->wait(10))
    //      yieldCurrentThread();
  }
}

/**
 *  Update status message.
 *
 *  @param[in] status New status.
 */
void failover::_update_status(std::string const& status) {
  std::lock_guard<std::mutex> lock(_status_m);
  _status = status;
}

uint32_t failover::_get_queued_events() const {
  return _subscriber->get_muxer().get_event_queue_size();
}

/**
 *  Start the internal thread.
 */
void failover::start() {
  log_v2::core()->trace("start failover '{}'.", _name);
  std::unique_lock<std::mutex> lock(_started_m);
  _stopped = false;
  if (!_started) {
    _should_exit = false;
    _thread = std::thread(&failover::_run, this);
    _started_cv.wait(lock, [this] { return _started; });
  }
  log_v2::core()->trace("failover '{}' started.", _name);
}

/**
 *  Check if bthread should exit.
 *
 *  @return True if bthread should exit.
 */
bool failover::should_exit() const {
  return _should_exit;
}
