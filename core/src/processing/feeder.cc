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
  : _client(client), _name(name), _subscriber(name, false) {
  _subscriber.get_muxer().set_read_filters(read_filters);
  _subscriber.get_muxer().set_write_filters(write_filters);
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
          timed_out_stream = !_client->read(d, 0);
        }
        catch (io::exceptions::shutdown const& e) {
          stream_can_read = false;
        }
      if (!d.isNull()) {
        _subscriber.get_muxer().write(d);
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
      if (!d.isNull())
        _client->write(d);

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
  }
  catch (...) {
    logging::error(logging::high)
      << "feeder: unknown error occured while processing client '"
      << _name << "'";
  }
  _client.clear();
  logging::info(logging::medium)
    << "feeder: thread of client '" << _name << "' will exit";
  return ;
}
