/*
** Copyright 2014 Merethis
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

#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <QProcess>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "config.hh"
#include "vars.hh"
#include "cbd.hh"
#include "generate.hh"
#include "misc.hh"
#include "vars.hh"

using namespace com::centreon::broker;

#define RECEIVER_FILE "fifo_dumper_receiver"
#define SENDER_FIFO "fifo_dumper_sender.cmd"

/**
 *  Check that the fifo dumper works.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);
  std::string fifo_config_path(tmpnam(NULL));
  std::string receiver_file = fifo_config_path + "/" + RECEIVER_FILE;
  std::string sender_fifo = fifo_config_path + "/" + SENDER_FIFO;
  cbd broker;

  std::cout << "receiver file: " << receiver_file << std::endl;
  std::cout << "sender fifo: " << sender_fifo << std::endl;

  try {
    ::mkdir(fifo_config_path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    // Create the config influxdb xml file.
    test_file file;
    file.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/fifo_dumper.xml.in");
    file.set("RECEIVER_FILE", receiver_file);
    file.set("SENDER_FIFO", sender_fifo);
    std::string config_file = file.generate();

    broker.set_config_file(config_file);
    broker.start();

    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    QProcess* process = new QProcess();
    std::string command = "./util_write_into_file \"test\n\" " + sender_fifo;
    process->start(QString::fromStdString(command));
    if (process->waitForStarted() == false)
      throw (exceptions::msg()
             << "can't start the process to write into the sending fifo "
             << process->errorString());

    sleep_for(10 * MONITORING_ENGINE_INTERVAL_LENGTH);

    std::ifstream ifs(receiver_file.c_str());
    if (!ifs.is_open())
      throw (exceptions::msg()
             << "can't open the receiving file");
    char buf[4096];
    ifs.getline(buf, 4096);
    if (ifs.fail())
      throw (exceptions::msg()
             << "can't read the receiving fifo");
    ifs.close();
    std::string got = buf;
    if (got != "test")
      throw (exceptions::msg()
             << "unexpected string received, got: " << got);

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();
  ::remove(receiver_file.c_str());
  ::remove(sender_fifo.c_str());
  ::rmdir(fifo_config_path.c_str());
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
