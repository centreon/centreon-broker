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

#define RECEIVER_DIRECTORY "dir_dumper_receiver"
#define SENDER_DIRECTORY "dir_dumper_sender"
#define SENT_FILENAME "dir_sent_file"

/**
 *  Check that the fifo dumper works.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);
  std::string fifo_config_path(tmpnam(NULL));
  std::string receiver_dir = fifo_config_path + "/" + RECEIVER_DIRECTORY;
  std::string expected_file = receiver_dir + "/" + SENT_FILENAME;
  std::string sender_dir = fifo_config_path + "/" + SENDER_DIRECTORY;
  std::string sent_file = sender_dir + "/" + SENT_FILENAME;
  cbd broker;

  std::cout << "receiver directory: " << receiver_dir << std::endl;
  std::cout << "sender directory: " << sender_dir << std::endl;

  try {
    ::mkdir(fifo_config_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    ::mkdir(receiver_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    ::mkdir(sender_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    // Create the config influxdb xml file.
    test_file file;
    file.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/directory_dumper.xml.in");
    file.set("SENDER_DIRECTORY", sender_dir);
    file.set("RECEIVER_DIRECTORY", receiver_dir);
    std::string config_file = file.generate();

    broker.set_config_file(config_file);
    broker.start();

    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    QProcess* process = new QProcess();
    std::string command = "./util_write_into_file \"test\n\" " + sent_file;
    process->start(QString::fromStdString(command));
    if (process->waitForStarted() == false)
      throw (exceptions::msg()
             << "can't start the process to write into the sent file "
             << process->errorString());

    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check for file existence.
    std::ifstream ifs(expected_file.c_str());
    if (!ifs.is_open())
      throw (exceptions::msg()
             << "can't open the expected file");
    char buf[4096];
    ifs.getline(buf, 4096);
    if (ifs.fail())
      throw (exceptions::msg()
             << "can't read the expected file");
    ifs.close();
    std::string got = buf;
    if (got != "test")
      throw (exceptions::msg()
             << "unexpected string received, got: " << got);

    // Remove file.
    ::remove(sent_file.c_str());
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check for file removal
    ifs.open(expected_file.c_str());
    if (ifs.is_open())
      throw (exceptions::msg()
             << "file '" << expected_file << "' not deleted");

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
  recursive_remove(fifo_config_path);
  recursive_remove(receiver_dir);
  recursive_remove(sender_dir);
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
