/*
** Copyright 2011-2012 Merethis
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
#include <QDir>
#include <QFile>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/ndo/internal.hh"
#include "com/centreon/broker/ndo/stream.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"

using namespace com::centreon::broker;

#define TEMP_FILE "broker_ndo_stream_erased_input"

/**
 *  Check that NDO layer works properly with erased input.
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
  ndo::initialize();

  // File path.
  QString path;
  path = QDir::tempPath();
  path.append("/" TEMP_FILE);
  QFile::remove(path);

  // File stream.
  misc::shared_ptr<file::stream>
    fs(new file::stream(path, QIODevice::ReadWrite));

  {
    // Buffer that will be written.
    misc::shared_ptr<io::raw> buffer(new io::raw);
    buffer->fill('\n', 8000);
    buffer->append(
      "42=21\n" \
      "36=12\n" \
      "999\n" \
      "\n");

    // Write buffer to file.
    fs->write(buffer.staticCast<io::data>());
  }

  // NDO stream.
  ndo::stream ns;
  ns.read_from(fs.staticCast<io::stream>());
  ns.write_to(fs.staticCast<io::stream>());

  // Sample object.
  misc::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);
  ack->acknowledgement_type = 1;
  ack->author = "Matthieu Kermagoret";
  ack->comment = "This is a comment.";
  ack->entry_time = 123456789;
  ack->host_id = 42;
  ack->instance_id = 21;
  ack->is_sticky = false;
  ack->notify_contacts = true;
  ack->persistent_comment = true;
  ack->service_id = 84;
  ack->state = 2;

  // Write to NDO stream.
  ns.write(ack.staticCast<io::data>());

  // Now the check begins.
  int retval(0);
  misc::shared_ptr<io::data> d;
  ns.read(d);
  if (d.isNull() || (d->type() != ack->type()))
    retval = 1;
  else {
    misc::shared_ptr<neb::acknowledgement>
      readack(d.staticCast<neb::acknowledgement>());
    retval = ((ack->acknowledgement_type
               != readack->acknowledgement_type)
              || (ack->author != readack->author)
              || (ack->comment != readack->comment)
              || (ack->entry_time != readack->entry_time)
              || (ack->host_id != readack->host_id)
              || (ack->instance_id != readack->instance_id)
              || (ack->is_sticky != readack->is_sticky)
              || (ack->notify_contacts != readack->notify_contacts)
              || (ack->persistent_comment
                  != readack->persistent_comment)
              || (ack->service_id != readack->service_id)
              || (ack->state != readack->state));
  }

  // Erase temporary file.
  ns.read_from(misc::shared_ptr<io::stream>());
  ns.write_to(misc::shared_ptr<io::stream>());
  fs.clear();
  QFile::remove(path);

  return (retval);
}
