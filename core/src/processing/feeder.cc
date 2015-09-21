/*
** Copyright 2011-2012 Centreon
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

#include <unistd.h>
#include <QReadLocker>
#include <QWriteLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/processing/feeder.hh"

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
 *  @param[in] name           Name.
 *  @param[in] client         Client stream.
 *  @param[in] read_filters   Read filters.
 *  @param[in] write_filters  Write filters.
 */
feeder::feeder(
          std::string const& name,
          misc::shared_ptr<io::stream> client,
          uset<unsigned int> const& read_filters,
          uset<unsigned int> const& write_filters)
  : thread(name), _client(client), _subscriber(name, false) {
  _subscriber.get_muxer().set_read_filters(read_filters);
  _subscriber.get_muxer().set_write_filters(write_filters);
  // By default, we assume the feeder is already connected.
  set_last_connection_times();
}

/**
 *  Destructor.
 */
feeder::~feeder() {}

/**
 *  Thread main routine.
 */
void feeder::run() {
  logging::info(logging::medium)
    << "feeder: thread of client '" << _name << "' is starting";
  try {
    if (_client.isNull())
      throw (exceptions::msg() << "could not process '"
             << _name << "' with no client stream");
    bool stream_can_read(true);
    bool muxer_can_read(true);
    misc::shared_ptr<io::data> d;
    while (!should_exit()) {
      // Read from stream.
      bool timed_out_stream(true);
      if (stream_can_read)
        try {
          QReadLocker lock(&_client_mutex);
          timed_out_stream = !_client->read(d, 0);
        }
        catch (io::exceptions::shutdown const& e) {
          stream_can_read = false;
        }
      if (!d.isNull()) {
        QReadLocker lock(&_client_mutex);
        _subscriber.get_muxer().write(d);
        _event_processing_speed.tick();
        continue ; // Stream read bias.
      }

      // Read from muxer.
      d.clear();
      bool timed_out_muxer(true);
      if (muxer_can_read)
        try {
          timed_out_muxer = !_subscriber.get_muxer().read(d, 0);
        }
        catch (io::exceptions::shutdown const& e) {
          muxer_can_read = false;
        }
      if (!d.isNull()) {
        QReadLocker lock(&_client_mutex);
        _client->write(d);
        _event_processing_speed.tick();
      }

      // If both timed out, sleep a while.
      d.clear();
      if (timed_out_stream && timed_out_muxer)
        ::usleep(100000);
    }
  }
  catch (io::exceptions::shutdown const& e) {
    // Normal termination.
    (void)e;
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "feeder: error occured while processing client '"
      << _name << "': " << e.what();
    _last_error = e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "feeder: unknown error occured while processing client '"
      << _name << "'";
  }
  {
    QWriteLocker lock(&_client_mutex);
    _client.clear();
  }
  logging::info(logging::medium)
    << "feeder: thread of client '" << _name << "' will exit";
  return ;
}

/**
 *  Get the state of the feeder.
 *
 *  @return  The state of the feeder.
 */
std::string feeder::_get_state() {
  char const* ret;
  if (_client_mutex.tryLockForRead(300)) {
    if (_client.isNull())
      ret = "disconnected";
    else
      ret = "connected";
    _client_mutex.unlock();
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
unsigned int feeder::_get_queued_events() {
  return (_subscriber.get_muxer().get_event_queue_size());
}

/**
 *  Get the read filters used by the feeder.
 *
 *  @return  The read filters used by the feeder.
 */
uset<unsigned int> feeder::_get_read_filters() {
  return (_subscriber.get_muxer().get_read_filters());
}

/**
 *  Get the write filters used by the feeder.
 *
 *  @return  The write filters used by the feeder.
 */
uset<unsigned int> feeder::_get_write_filters() {
  return (_subscriber.get_muxer().get_write_filters());
}

/**
 *  Forward to stream.
 *
 *  @param[in] tree  The statistic tree.
 */
void feeder::_forward_statistic(io::properties& tree) {
  if (_client_mutex.tryLockForRead(300)) {
    if (!_client.isNull())
      _client->statistics(tree);
    _client_mutex.unlock();
  }
  _subscriber.get_muxer().statistics(tree);
}
