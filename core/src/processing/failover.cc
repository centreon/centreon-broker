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
    : bthread(name),
      _buffering_timeout(0),
      _endpoint(endp),
      _failover_launched(false),
      _initialized(false),
      _next_timeout(0),
      _retry_interval(30),
      _subscriber(sbscrbr),
      _update(false) {}

/**
 *  Destructor.
 */
failover::~failover() {}

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
  bthread::exit();
  _subscriber->get_muxer().wake();
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
void failover::run() {
  // Initial log.
  logging::debug(logging::high)
      << "failover: thread of endpoint '" << _name << "' is starting";

  // Check endpoint.
  if (!_endpoint) {
    logging::error(logging::high)
        << "failover: thread of endpoint '" << _name << "' has no endpoint"
        << " object, this is likely a software bug that should be reported"
        << " to Centreon Broker developers";
    return;
  }

  // Thread should be aware of external exit requests.
  do {
    // This try/catch block handle any errors of the current thread
    // objects. In case of an exception, it is responsible to launch
    // failovers of this failover.
    try {
      // Attempt to open endpoint.
      _update_status("opening endpoint");
      set_last_connection_attempt(timestamp::now());
      {
        std::shared_ptr<io::stream> s(_endpoint->open());
        {
          std::lock_guard<std::timed_mutex> stream_lock(_streamm);
          _stream = s;
        }
        _initialized = true;
        set_last_connection_success(timestamp::now());
      }
      _update_status("");
      _update = true;

      // Buffering.
      if (_buffering_timeout > 0) {
        // Status.
        logging::debug(logging::medium)
            << "failover: buffering data for endpoint '" << _name << "' ("
            << _buffering_timeout << "s)";
        _update_status("buffering data");

        // Wait loop.
        // FIXME DBR: attempt to replace the Qt code below.
        if (!should_exit())
          std::this_thread::sleep_for(std::chrono::seconds(_buffering_timeout));
        //        return;

        //        time_t valid_time(time(NULL) + _buffering_timeout);
        //        do {
        //          QTimer::singleShot(1000, this, SLOT(quit()));
        //          exec();
        //        } while (!should_exit() && (time(NULL) < valid_time));

        // FIXME DBR: I don't see how this method could be called... even
        // in the Qt version.
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
        logging::debug(logging::medium)
            << "failover: shutting down failover of endpoint '" << _name << "'";
        _update_status("shutting down failover");
        _failover->exit();
        _failover->wait();
        _failover_launched = false;
        _update_status("");
      }

      // Event processing loop.
      logging::debug(logging::medium)
          << "failover: launching event loop of endpoint '" << _name << "'";
      _subscriber->get_muxer().nack_events();
      bool stream_can_read(true);
      bool muxer_can_read(true);
      bool should_commit(false);
      std::shared_ptr<io::data> d;
      while (!should_exit()) {
        // Process events.
        // QCoreApplication::processEvents();

        // Check for update.
        if (_update) {
          std::lock_guard<std::timed_mutex> stream_lock(_streamm);
          _stream->update();
          _update = false;
        }

        // Read from endpoint stream.
        d.reset();
        bool timed_out_stream(true);
        if (stream_can_read) {
          logging::debug(logging::low)
              << "failover: reading event from endpoint '" << _name << "'";
          _update_status("reading event from stream");
          try {
            std::lock_guard<std::timed_mutex> stream_lock(_streamm);
            timed_out_stream = !_stream->read(d, 0);
          } catch (exceptions::shutdown const& e) {
            logging::debug(logging::medium)
                << "failover: stream of endpoint '" << _name
                << "' shutdown while reading: " << e.what();
            stream_can_read = false;
          }
          if (d) {
            logging::debug(logging::low)
                << "failover: writing event of endpoint '" << _name
                << "' to multiplexing engine";
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
          logging::debug(logging::low) << "failover: reading event from "
                                          "multiplexing engine for endpoint '"
                                       << _name << "'";
          _update_status("reading event from multiplexing engine");
          try {
            timed_out_muxer = !_subscriber->get_muxer().read(d, 0);
            should_commit = should_commit || d;
          } catch (exceptions::shutdown const& e) {
            logging::debug(logging::medium)
                << "failover: muxer of endpoint '" << _name
                << "' shutdown while reading: " << e.what();
            muxer_can_read = false;
          }
          if (d) {
            logging::debug(logging::low) << "failover: writing event of "
                                            "multiplexing engine to endpoint '"
                                         << _name << "'";
            _update_status("writing event to stream");
            int we(0);
            try {
              std::lock_guard<std::timed_mutex> stream_lock(_streamm);
              we = _stream->write(d);
            } catch (exceptions::shutdown const& e) {
              logging::debug(logging::medium)
                  << "failover: stream of endpoint '" << _name
                  << "' shutdown while writing: " << e.what();
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
            std::lock_guard<std::timed_mutex> stream_lock(_streamm);
            we = _stream->flush();
          } else if (now >= _next_timeout) {
            _next_timeout = now + 1;
            std::lock_guard<std::timed_mutex> stream_lock(_streamm);
            we = _stream->flush();
          }
          _subscriber->get_muxer().ack_events(we);
          ::usleep(100000);
        }
      }
    }
    // Some real error occured.
    catch (std::exception const& e) {
      logging::error(logging::high) << e.what();
      {
        std::lock_guard<std::timed_mutex> stream_lock(_streamm);
        _stream.reset();
      }
      _launch_failover();
      _initialized = true;
    } catch (...) {
      logging::error(logging::high)
          << "failover: endpoint '" << _name
          << "' encountered an unknown exception, this is likely a "
          << "software bug that should be reported to Centreon Broker "
             "developers";
      {
        std::lock_guard<std::timed_mutex> stream_lock(_streamm);
        _stream.reset();
      }
      _launch_failover();
      _initialized = true;
    }

    // Clear stream.
    {
      std::lock_guard<std::timed_mutex> stream_lock(_streamm);
      _stream.reset();
    }

    // Sleep a while before attempting a reconnection.
    _update_status("sleeping before reconnection");

    // FIXME DBR: attempt to replace the Qt code below.
    if (!should_exit())
      std::this_thread::sleep_for(std::chrono::seconds(_retry_interval));

    //    time_t valid_time(time(NULL) + _retry_interval);
    //    while (!should_exit() && (time(NULL) < valid_time)) {
    //      QTimer::singleShot(1000, this, SLOT(quit()));
    //      exec();
    //    }
    // FIXME DBR: I don't see how this method could be called... even
    // in the Qt version.
    _update_status("");

  } while (!should_exit());

  // Clear stream.
  {
    std::lock_guard<std::timed_mutex> stream_lock(_streamm);
    _stream.reset();
  }

  // Exit failover thread if necessary.
  if (_failover) {
    logging::info(logging::medium)
        << "failover: requesting termination of failover of endpoint '" << _name
        << "'";
    _failover->exit();
  }

  // Exit log.
  logging::debug(logging::high)
      << "failover: thread of endpoint '" << _name << "' is exiting";
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
bool failover::wait(unsigned long time) {
  // Check that failover finished.
  bool finished;
  if (_failover)
    finished = _failover->wait(time);
  else
    finished = true;

  // If there was no failover or failover finished we
  // can safely wait for ourselves.
  if (finished)
    finished = bthread::wait(time);
  // Otherwise we're not finished yet.
  else
    finished = false;
  return finished;
}

/**
 *  Get the state of the failover.
 *
 *  @return  The state of the failover.
 */
const char* failover::_get_state() const {
  char const* ret = nullptr;
  if (_streamm.try_lock_for(std::chrono::milliseconds(10))) {
    if (!_stream)
      ret = "connecting";
    else
      ret = "connected";
    _streamm.unlock();
  } else
    ret = "blocked";
  return ret;
}

/**
 *  Get the number of queued events.
 *
 *  @return  The number of queued events.
 */
uint32_t failover::_get_queued_events() {
  return _subscriber->get_muxer().get_event_queue_size();
}

/**
 *  Get the read filters used by the failover.
 *
 *  @return  The read filters used by the failover.
 */
std::unordered_set<uint32_t> const& failover::_get_read_filters() const {
  return _subscriber->get_muxer().get_read_filters();
}

/**
 *  Get the write filters used by the failover.
 *
 *  @return  The write filters used by the failover.
 */
std::unordered_set<uint32_t> const& failover::_get_write_filters() const {
  return _subscriber->get_muxer().get_write_filters();
}

/**
 *  Forward to failover.
 *
 *  @param[in] tree  The tree.
 */
void failover::_forward_statistic(io::properties& tree) {
  {
    std::lock_guard<std::mutex> lock(_statusm);
    tree.add_property("status", io::property("status", _status));
  }
  {
    std::unique_lock<std::timed_mutex> stream_lock(_streamm, std::defer_lock);
    if (stream_lock.try_lock_for(std::chrono::milliseconds(100))) {
      if (_stream)
        _stream->statistics(tree);
    } else
      tree.add_property("status", io::property("status", "busy"));
  }
  _subscriber->get_muxer().statistics(tree);
  io::properties subtree;
  if (_failover)
    _failover->stats(subtree);
  tree.add_child(subtree, "failover");
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

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
  std::lock_guard<std::mutex> lock(_statusm);
  _status = status;
}
