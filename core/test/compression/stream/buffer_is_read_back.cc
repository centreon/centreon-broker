/*
** Copyright 2014-2015 Merethis
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_compression_buffer_is_read_back.tmp"

/**
 *  Fake stream that throws when used.
 */
class          fake_stream : public io::stream {
public:
  bool         read(misc::shared_ptr<io::data>& d, time_t deadline) {
    (void)d;
    (void)deadline;
    throw (io::exceptions::shutdown(true, true)
           << "fake stream never reads");
    return (true);
  }

  unsigned int write(misc::shared_ptr<io::data> const& d) {
    (void)d;
    throw (io::exceptions::shutdown(true, true)
           << "fake stream never writes");
    return (0);
  }
};

/**
 *  Check that internal compression buffer is properly read back after
 *  being written.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Log on stderr.
  logging::file f(stderr);
  logging::manager::instance().log_on(
                                 f,
                                 logging::config_type
                                 | logging::debug_type
                                 | logging::error_type
                                 | logging::info_type,
                                 logging::low);

  // Compression stream.
  misc::shared_ptr<fake_stream> fs(new fake_stream);
  compression::stream cs(-1, 40000);
  cs.set_substream(fs);

  // Write data to compression stream.
  {
    misc::shared_ptr<io::raw> data(new io::raw);
    data->append("0123456789abcdefghijklmnopqrstuvwxyz");
    cs.write(data);
  }

  // Read data back and check it.
  bool error(true);
  {
    misc::shared_ptr<io::data> data;
    cs.read(data);
    if (!data.isNull()
        && (io::events::data_type<io::events::internal, 1>::value
            == data->type())) {
      misc::shared_ptr<io::raw> raw;
      raw = data.staticCast<io::raw>();
      if (!memcmp(
             raw->QByteArray::data(),
             "0123456789abcdefghijklmnopqrstuvwxyz",
             36))
        error = false;
    }
  }

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
