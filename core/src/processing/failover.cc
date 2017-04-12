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

#include <QCoreApplication>
#include <QTimer>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/processing/failover.hh"

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
failover::failover(
            misc::shared_ptr<io::endpoint> endp,
            misc::shared_ptr<multiplexing::subscriber> sbscrbr,
            std::string const& name)
  : thread(name),
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
void failover::add_secondary_endpoint(
                 misc::shared_ptr<io::endpoint> endp) {
  _secondary_endpoints.push_back(endp);
  return ;
}

/**
 *  Exit failover thread.
 */
void failover::exit() {
  thread::exit();
  _subscriber->get_muxer().wake();
  return ;
}

/**
 *  Get buffering timeout.
 *
 *  @return Failover thread buffering timeout.
 */
time_t failover::get_buffering_timeout() const throw () {
  return (_buffering_timeout);
}

/**
 *  Check whether or not the thread has initialize.
 *
 *  @return True if the thread is initializable. That is it is read()able.
 */
bool failover::get_initialized() const throw () {
  return (_initialized);
}

/**
 *  Get retry interval.
 *
 *  @return Failover thread retry interval.
 */
time_t failover::get_retry_interval() const throw () {
  return (_retry_interval);
}

/**
 *  Thread core function.
 */
void failover::run() {
  // Initial log.
  logging::debug(logging::high)
    << "failover: thread of endpoint '" << _name << "' is starting";

  // Check endpoint.
  if (_endpoint.isNull()) {
    logging::error(logging::high)
      << "failover: thread of endpoint '" << _name << "' has no endpoint"
      << " object, this is likely a software bug that should be reported"
      << " to Centreon Broker developers";
    return ;
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
        misc::shared_ptr<io::stream> s(_endpoint->open());
        {
          QMutexLocker stream_lock(&_streamm);
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
          << "failover: buffering data for endpoint '" << _name
          << "' (" << _buffering_timeout << "s)";
        _update_status("buffering data");

        // Wait loop.
        time_t valid_time(time(NULL) + _buffering_timeout);
        do {
          QTimer::singleShot(1000, this, SLOT(quit()));
          exec();
        } while (!should_exit() && (time(NULL) < valid_time));
        _update_status("");
      }

      // Open secondaries.
      _update_status("initializing secondaries");
      std::vector<misc::shared_ptr<io::stream> > secondaries;
      for (std::vector<misc::shared_ptr<io::endpoint> >::iterator
             it(_secondary_endpoints.begin()),
             end(_secondary_endpoints.end());
           it != end;
           ++it)
        try {
          misc::shared_ptr<io::stream> s((*it)->open());
          if (!s.isNull())
            secondaries.push_back(s);
          else
            logging::error(logging::medium)
              << "failover: could not open a secondary of endpoint '"
              << _name << ": secondary returned a null stream";
        }
        catch (std::exception const& e) {
          logging::error(logging::medium)
            << "failover: error occured while opening a secondary "
            << "of endpoint '" << _name << "': " << e.what();
        }
      _update_status("");

      // Shutdown failover.
      if (_failover_launched) {
        logging::debug(logging::medium)
          << "failover: shutting down failover of endpoint '"
          << _name << "'";
        _update_status("shutting down failover");
        _failover->exit();
        _failover->wait();
        _failover_launched = false;
        _update_status("");
      }

      // Event processing loop.
      logging::debug(logging::medium)
        << "failover: launching event loop of endpoint '"
        << _name << "'";
      _subscriber->get_muxer().nack_events();
      bool stream_can_read(true);
      bool muxer_can_read(true);
      bool should_commit(false);
      misc::shared_ptr<io::data> d;
      while (!should_exit()) {
        // Process events.
        QCoreApplication::processEvents();

        // Check for update.
        if (_update) {
          QMutexLocker stream_lock(&_streamm);
          _stream->update();
          _update = false;
        }

        // Read from endpoint stream.
        d.clear();
        bool timed_out_stream(true);
        if (stream_can_read) {
          logging::debug(logging::low)
            << "failover: reading event from endpoint '"
            << _name << "'";
          _update_status("reading event from stream");
          try {
            QMutexLocker stream_lock(&_streamm);
            timed_out_stream = !_stream->read(d, 0);
          }
          catch (exceptions::shutdown const& e) {
            logging::debug(logging::medium)
              << "failover: stream of endpoint '" << _name
              << "' shutdown while reading: " << e.what();
            stream_can_read = false;
          }
          if (!d.isNull()) {
            logging::debug(logging::low)
              << "failover: writing event of endpoint '" << _name
              << "' to multiplexing engine";
            _update_status("writing event to multiplexing engine");
            _subscriber->get_muxer().write(d);
            tick();
            _update_status("");
            continue ; // Stream read bias.
          }
          _update_status("");
        }

        // Read from muxer stream.
        d.clear();
        bool timed_out_muxer(true);
        if (muxer_can_read) {
          logging::debug(logging::low)
            << "failover: reading event from multiplexing engine for endpoint '"
            << _name << "'";
          _update_status("reading event from multiplexing engine");
          try {
            timed_out_muxer = !_subscriber->get_muxer().read(d, 0);
            should_commit = should_commit || !d.isNull();
          }
          catch (exceptions::shutdown const& e) {
            logging::debug(logging::medium)
              << "failover: muxer of endpoint '" << _name
              << "' shutdown while reading: " << e.what();
            muxer_can_read = false;
          }
          if (!d.isNull()) {
            logging::debug(logging::low)
              << "failover: writing event of multiplexing engine to endpoint '"
              << _name << "'";
            _update_status("writing event to stream");
            int we(0);
            try {
              QMutexLocker stream_lock(&_streamm);
              we = _stream->write(d);
            }
            catch (exceptions::shutdown const& e) {
              logging::debug(logging::medium)
                << "failover: stream of endpoint '" << _name
                << "' shutdown while writing: " << e.what();
              muxer_can_read = false;
            }
            _subscriber->get_muxer().ack_events(we);
            tick();
            for (std::vector<misc::shared_ptr<io::stream> >::iterator
                   it(secondaries.begin()),
                   end(secondaries.end());
                 it != end;) {
              try {
                (*it)->write(d);
                ++it;
              }
              catch (std::exception const& e) {
                logging::error(logging::medium) << "failover: error "
                  << "occurred while writing to a secondary of endpoint '"
                  << _name << "' (secondary will be removed): "
                  << e.what();
                it = secondaries.erase(it);
              }
            }
            _update_status("");
          }
        }

        // If both timed out, sleep a while.
        d.clear();
        if (timed_out_stream && timed_out_muxer) {
          time_t now(time(NULL));
          int we(0);
          if (should_commit) {
            should_commit = false;
            _next_timeout = now + 1;
            QMutexLocker stream_lock(&_streamm);
            we = _stream->flush();
          }
          else if (now >= _next_timeout) {
            _next_timeout = now + 1;
            QMutexLocker stream_lock(&_streamm);
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
        QMutexLocker stream_lock(&_streamm);
        _stream.clear();
      }
      _launch_failover();
      _initialized = true;
    }
    catch (...) {
      logging::error(logging::high) << "failover: endpoint '" << _name
        << "' encountered an unknown exception, this is likely a "
        << "software bug that should be reported to Centreon Broker developers";
      {
        QMutexLocker stream_lock(&_streamm);
        _stream.clear();
      }
      _launch_failover();
      _initialized = true;
    }

    // Clear stream.
    {
      QMutexLocker stream_lock(&_streamm);
      _stream.clear();
    }

    // Sleep a while before attempting a reconnection.
    _update_status("sleeping before reconnection");
    time_t valid_time(time(NULL) + _retry_interval);
    while (!should_exit() && (time(NULL) < valid_time)) {
      QTimer::singleShot(1000, this, SLOT(quit()));
      exec();
    }
    _update_status("");

  } while (!should_exit());

  // Clear stream.
  {
    QMutexLocker stream_lock(&_streamm);
    _stream.clear();
  }

  // Exit failover thread if necessary.
  if (_failover.data()) {
    logging::info(logging::medium)
      << "failover: requesting termination of failover of endpoint '"
      << _name << "'";
    _failover->exit();
  }

  // Exit log.
  logging::debug(logging::high)
    << "failover: thread of endpoint '" << _name << "' is exiting";

  return ;
}

/**
 *  Set buffering timeout.
 *
 *  @param[in] secs Buffering timeout in seconds.
 */
void failover::set_buffering_timeout(time_t secs) {
  _buffering_timeout = secs;
  return ;
}

/**
 *  Set the thread's failover.
 *
 *  @param[in] fo Thread's failover.
 */
void failover::set_failover(misc::shared_ptr<failover> fo) {
  _failover = fo;
  return ;
}

/**
 *  Set the connection retry interval.
 *
 *  @param[in] retry_interval Time to wait between two connection
 *                            attempts.
 */
void failover::set_retry_interval(time_t retry_interval) {
  _retry_interval = retry_interval;
  return ;
}

/**
 *  Configuration update request.
 */
void failover::update() {
  _update = true;
  return ;
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
  if (!_failover.isNull())
    finished = _failover->wait(time);
  else
    finished = true;

  // If there was no failover or failover finished we
  // can safely wait for ourselves.
  if (finished)
    finished = thread::wait(time);
  // Otherwise we're not finished yet.
  else
    finished = false;
  return (finished);
}

/**
 *  Get the state of the failover.
 *
 *  @return  The state of the failover.
 */
std::string failover::_get_state() {
  char const* ret = NULL;
  if (_streamm.tryLock()) {
    if (_stream.isNull())
      ret = "connecting";
    else
      ret = "connected";
    _streamm.unlock();
  }
  else
    ret = "blocked";
  return (ret);
}

/**
 *  Get the number of queued events.
 *
 *  @return  The number of queued events.
 */
unsigned int failover::_get_queued_events() {
  return (_subscriber->get_muxer().get_event_queue_size());
}

/**
 *  Get the read filters used by the failover.
 *
 *  @return  The read filters used by the failover.
 */
uset<unsigned int> failover::_get_read_filters() {
  return (_subscriber->get_muxer().get_read_filters());
}

/**
 *  Get the write filters used by the failover.
 *
 *  @return  The write filters used by the failover.
 */
uset<unsigned int> failover::_get_write_filters() {
  return (_subscriber->get_muxer().get_write_filters());
}

/**
 *  Forward to failover.
 *
 *  @param[in] tree  The tree.
 */
void failover::_forward_statistic(io::properties& tree) {
  {
    QMutexLocker lock(&_statusm);
    tree.add_property("status", io::property("status", _status));
  }
  {
    QMutexLocker lock(&_streamm);
    if (!_stream.isNull())
      _stream->statistics(tree);
  }
  _subscriber->get_muxer().statistics(tree);
  io::properties subtree;
  if (!_failover.isNull())
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
  if (!_failover.isNull() && !_failover_launched) {
    _failover_launched = true;
    _failover->start();
    while (!_failover->get_initialized() && !_failover->wait(10))
      yieldCurrentThread();
  }
  return ;
}

/**
 *  Update status message.
 *
 *  @param[in] status New status.
 */
void failover::_update_status(std::string const& status) {
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}
