/*
** Copyright 2011-2012,2015,2017 Centreon
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

#include "com/centreon/broker/processing/feeder.hh"
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"

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
feeder::feeder(std::string const& name,
               std::shared_ptr<io::stream> client,
               uset<unsigned int> const& read_filters,
               uset<unsigned int> const& write_filters)
    : bthread(name), _client(client), _subscriber(name, false) {
  _subscriber.get_muxer().set_read_filters(read_filters);
  _subscriber.get_muxer().set_write_filters(write_filters);
  // By default, we assume the feeder is already connected.
  set_last_connection_attempt(timestamp::now());
  set_last_connection_success(timestamp::now());
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
    if (!_client)
      throw(exceptions::msg()
            << "could not process '" << _name << "' with no client stream");
    bool stream_can_read(true);
    bool muxer_can_read(true);
    std::shared_ptr<io::data> d;
    while (!should_exit()) {
      // Read from stream.
      bool timed_out_stream(true);
      if (stream_can_read) {
        try {
          misc::read_lock lock(_client_mutex);
          timed_out_stream = !_client->read(d, 0);
        } catch (exceptions::shutdown const& e) {
          stream_can_read = false;
        }
        if (d) {
          {
            misc::read_lock lock(_client_mutex);
            _subscriber.get_muxer().write(d);
          }
          tick();
          continue;  // Stream read bias.
        }
      }

      // Read from muxer.
      d.reset();
      bool timed_out_muxer(true);
      if (muxer_can_read)
        try {
          timed_out_muxer = !_subscriber.get_muxer().read(d, 0);
        } catch (exceptions::shutdown const& e) {
          muxer_can_read = false;
        }
      if (d) {
        {
          misc::read_lock lock(_client_mutex);
          _client->write(d);
        }
        _subscriber.get_muxer().ack_events(1);
        tick();
      }

      // If both timed out, sleep a while.
      d.reset();
      if (timed_out_stream && timed_out_muxer)
        ::usleep(100000);
    }
  } catch (exceptions::shutdown const& e) {
    // Normal termination.
    (void)e;
  } catch (std::exception const& e) {
    logging::error(logging::medium)
        << "feeder: error occured while processing client '" << _name
        << "': " << e.what();
    set_last_error(e.what());
  } catch (...) {
    logging::error(logging::high)
        << "feeder: unknown error occured while processing client '" << _name
        << "'";
  }
  {
    misc::read_lock lock(_client_mutex);
    _client.reset();
    _subscriber.get_muxer().remove_queue_files();
  }
  logging::info(logging::medium)
      << "feeder: thread of client '" << _name << "' will exit";
}

/**
 *  Get the state of the feeder.
 *
 *  @return  The state of the feeder.
 */
std::string feeder::_get_state() {
  char const* ret;
  if (_client_mutex.try_lock_shared_for(300)) {
    if (!_client)
      ret = "disconnected";
    else
      ret = "connected";
    _client_mutex.unlock();
  } else
    ret = "blocked";
  return ret;
}

/**
 *  Get the number of queued events.
 *
 *  @return  The number of queued events.
 */
unsigned int feeder::_get_queued_events() {
  return _subscriber.get_muxer().get_event_queue_size();
}

/**
 *  Get the read filters used by the feeder.
 *
 *  @return  The read filters used by the feeder.
 */
uset<unsigned int> feeder::_get_read_filters() {
  return _subscriber.get_muxer().get_read_filters();
}

/**
 *  Get the write filters used by the feeder.
 *
 *  @return  The write filters used by the feeder.
 */
uset<unsigned int> feeder::_get_write_filters() {
  return _subscriber.get_muxer().get_write_filters();
}

/**
 *  Forward to stream.
 *
 *  @param[in] tree  The statistic tree.
 */
void feeder::_forward_statistic(io::properties& tree) {
  if (_client_mutex.try_lock_shared_for(300)) {
    if (_client)
      _client->statistics(tree);
    _client_mutex.unlock();
  }
  _subscriber.get_muxer().statistics(tree);
}
