/*
** Copyright 2015 Centreon
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

#include <arpa/inet.h>
#include <iostream>
#include <bitset>
#include <QCoreApplication>
#include <QDir>
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

// Valid empty event (a command_request).
#define EMPTY_EVENT "000E000800020000000000000000000000000000"
// This event does not have enough data for its payload.
#define NOT_ENOUGH_DATA_EVENT "0009000800020000000000000000000000000000"
// This event is of invalid type.
#define INVALID_EVENT_TYPE "000000420042"

class from_memory : public io::stream {
public:
  from_memory(QByteArray const& memory)
    : sent_data(false), _memory(memory) {}
  ~from_memory() {}

  bool read(
         std::shared_ptr<io::data>& d,
         time_t deadline = (time_t)-1) {
    (void)deadline;
    if (sent_data)
      throw (io::exceptions::shutdown(true, true));
    std::shared_ptr<io::raw> raw(new io::raw);
    raw.ref_as<QByteArray>() = _memory;
    d = raw;
    sent_data = true;
    return (true);
  }

  int write(std::shared_ptr<io::data> const& d) {
    (void)d;
    throw (io::exceptions::shutdown(false, true));
    return (-1);
  }

private:
  bool sent_data;
  QByteArray _memory;
};

/**
 *  Compute and prepend the checksum to a packet.
 *
 *  @param packet  The packet.
 */
void prepend_checksum(QByteArray& packet) {
  quint16 checksum = htons(qChecksum(packet.data(), 6));
  packet.prepend((char*)&checksum, sizeof(quint16));
}

/**
 *  Append an empty event at the end of the packet.
 *  @param[in] packet  The empty raw event.
 */
void append_empty_valid_event(QByteArray& packet) {
  QByteArray packet2 = QByteArray::fromHex(EMPTY_EVENT);

  prepend_checksum(packet2);
  packet.append(packet2);
}

/**
 *  Test a packet containing not enough data.
 */
void test_not_enough_data() {
  try {
    QByteArray packet = QByteArray::fromHex(NOT_ENOUGH_DATA_EVENT);
    prepend_checksum(packet);
    std::shared_ptr<from_memory> memory_stream(new from_memory(packet));
    bbdo::stream stream;
    stream.set_substream(memory_stream);
    // We should be throwing.
    try {
      std::shared_ptr<io::data> d;
      stream.read(d);
    } catch (io::exceptions::shutdown const& e) {}
      catch (std::exception const& e) {
      return ;
    }
  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "couldn't initialize streams for test 'not_enough_data'");
  }
  throw (exceptions::msg()
         << "'not_enough_data' test didn't throw");
}

/**
 *  Test an event of invalid type.
 */
void test_invalid_event() {
  try {
    QByteArray packet = QByteArray::fromHex(INVALID_EVENT_TYPE);
    prepend_checksum(packet);
    append_empty_valid_event(packet);
    std::shared_ptr<from_memory> memory_stream(new from_memory(packet));
    bbdo::stream stream;
    stream.set_coarse(true);
    stream.set_substream(memory_stream);
    std::shared_ptr<io::data> d;
    // The first is null.
    stream.read(d);
    // The second is an extcmd.
    stream.read(d);
    if (d.isNull() || d->type() != extcmd::command_request::static_type())
      throw (exceptions::msg() << "expected a command request event");
  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "error while executing 'invalid_event_type' test: " << e.what());
  }
}

/**
 *  Test an event of with bad checksum.
 */
void test_invalid_checksum() {
  try {
    QByteArray packet = QByteArray::fromHex(EMPTY_EVENT);
    // Bogus checksum.
    packet.prepend(42);
    packet.prepend(42);
    append_empty_valid_event(packet);
    std::shared_ptr<from_memory> memory_stream(new from_memory(packet));
    bbdo::stream stream;
    stream.set_coarse(true);
    stream.set_substream(memory_stream);
    std::shared_ptr<io::data> d;
    stream.read(d);
    if (d.isNull() || d->type() != extcmd::command_request::static_type())
      throw (exceptions::msg() << "expected a command request event");
  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "error while executing 'invalid_checksum' test: " << e.what());
  }
}

/**
 *  Check that file stream can be properly written to.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();
  extcmd::load();

  int retval = 0;

  try {
    test_not_enough_data();
    test_invalid_event();
    test_invalid_checksum();
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    retval = -1;
  }

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
