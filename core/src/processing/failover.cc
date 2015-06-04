/*
** Copyright 2011-2015 Merethis
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

#include <QCoreApplication>
#include <QTimer>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
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
 *  @param[in] temp_dir  Temporary directory.
 */
failover::failover(
            misc::shared_ptr<io::endpoint> endp,
            misc::shared_ptr<multiplexing::subscriber> sbscrbr,
            QString const& name,
            std::string const& temp_dir)
  : _buffering_timeout(0),
    _endpoint(endp),
    _failover_launched(false),
    _name(name),
    _retry_interval(30),
    _subscriber(sbscrbr),
    _temp_dir(temp_dir),
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
 *  Get retry interval.
 *
 *  @return Failover thread retry interval.
 */
time_t failover::get_retry_interval() const throw () {
  return (_retry_interval);
}

/**
 *  Read data.
 *
 *  @param[out] data      Data.
 *  @param[in]  deadline  Timeout.
 */
bool failover::read(
                 misc::shared_ptr<io::data>& data,
                 time_t deadline) {
  // The read() method is used by external objects to read from the main
  // stream contained in this object. Typically this method is called
  // by another failover object that uses this object as its failover
  // endpoint.

  // First we try to read from the main stream.
  QMutexLocker stream_lock(&_streamm);
  if (!_stream.isNull()) {
    try {
      return (_stream->read(data, deadline));
    }
    catch (std::exception const& e) {
      // In the run() method, it is guaranteed that no more write will
      // occur on the stream if thread exit was requested. It is
      // important to unlock the stream lock only after setting the exit
      // flag.
      if (isRunning()) {
        exit();
        stream_lock.unlock();
        wait();
      }
      else {
        _stream.clear();
        stream_lock.unlock();
      }
      logging::info(logging::high)
        << "failover: endpoint '" << _name
        << "' main stream cannot be read anymore, will try failover: "
        << e.what();

      // Now that the stream is cleared and mutex released, try to read
      // from failover.
      return (read(data, deadline));
    }
  }
  // If the main stream was not ready to provide events, try the
  // failover thread.
  else {
    stream_lock.unlock();
    if (!_failover.isNull())
      return (_failover->read(data, deadline));
    else
      throw (io::exceptions::shutdown(true, true)
             << "failover: endpoint '" << _name
             << "' does not have further events");
  }
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

  // Initial launch of failovers (to read retained data).
  logging::debug(logging::medium)
    << "failover: initializing failovers of endpoint '" << _name << "'";
  _update_status("initializing failovers");
  _launch_failover();
  _update_status("");

  // Thread should be aware of external exit requests.
  do {
    // This try/catch block handle any errors of the current thread
    // objects. In case of an exception, it is responsible to launch
    // failovers of this failover.
    try {
      // Attempt to open endpoint.
      _update_status("opening endpoint");
      {
        misc::shared_ptr<io::stream> s(_endpoint->open());
        {
          QMutexLocker stream_lock(&_streamm);
          _stream = s;
        }
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

      // Recovery loop.
      if (_failover_launched) {
        logging::debug(logging::medium)
          << "failover: recovering data from failover";
        _update_status("recovering data from failover");
        try {
          misc::shared_ptr<io::data> d;
          while (!should_exit()) {
            // XXX : event acknowledgement
            bool timed_out(false);
            _failover->read(d, 0, &timed_out);
            if (timed_out)
              break ;
            _stream->write(d);
          }
        }
        catch (io::exceptions::shutdown const& e) {
          // Normal termination.
          (void)e;
        }
        // Shutdown failover.
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
      bool stream_can_read(true);
      bool muxer_can_read(true);
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
          // XXX : event acknowledgement
          _update_status("reading event from stream");
          try {
            QMutexLocker stream_lock(&_streamm);
            _stream->read(d, 0, &timed_out_stream);
          }
          catch (io::exceptions::shutdown const& e) {
            logging::debug(logging::medium)
              << "failover: stream of endpoint '" << _name
              << "' shutdown while reading: " << e.what();
            stream_can_read = false;
          }
          if (!d.isNull()) {
            _subscriber->get_muxer().write(d);
            continue ; // Stream read bias.
          }
        }

        // Read from muxer stream.
        bool timed_out_muxer(true);
        if (muxer_can_read) {
          try {
            _subscriber->get_muxer().read(d, 0, &timed_out_muxer);
          }
          catch (io::exceptions::shutdown const& e) {
            logging::debug(logging::medium)
              << "failover: muxer of endpoint '" << _name
              << "' shutdown while reading: " << e.what();
            muxer_can_read = false;
          }
          if (!d.isNull()) {
            // XXX : event acknowledgement
            _update_status("writing event to stream");
            {
              QMutexLocker stream_lock(&_streamm);
              _stream->write(d);
            }
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
          }
        }

        // If both timed out, sleep a while.
        d.clear();
        if (timed_out_stream && timed_out_muxer)
          ::usleep(100000);
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
 *  Get statistics of the failover.
 *
 *  @param[out] tree  Properties tree.
 */
void failover::statistics(io::properties& tree) const {
  // XXX
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
 *  Write data.
 *
 *  @param[in] d  Unused.
 *
 *  @return Does not return, throw an exception.
 */
unsigned int failover::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (exceptions::msg() << "cannot write to endpoint '"
         << _name << "'");
  return (0);
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
  if (!_failover.isNull() && !_failover_launched) {
    _failover_launched = true;
    _failover->start();
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
