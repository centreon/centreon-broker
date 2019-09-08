/*
** Copyright 2014-2015 Centreon
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <QProcess>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include "cbd.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "config.hh"
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
    ::mkdir(fifo_config_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    // Create the config influxdb xml file.
    test_file file;
    file.set_template(PROJECT_SOURCE_DIR "/test/cfg/fifo_dumper.xml.in");
    file.set("RECEIVER_FILE", receiver_file);
    file.set("SENDER_FIFO", sender_fifo);
    std::string config_file = file.generate();

    broker.set_config_file(config_file);
    broker.start();

    sleep_for(3);

    QProcess* process = new QProcess();
    std::string command = "./util_write_into_file \"test\n\" " + sender_fifo;
    process->start(QString::fromStdString(command));
    if (process->waitForStarted() == false)
      throw(exceptions::msg()
            << "can't start the process to write into the sending fifo "
            << process->errorString());

    sleep_for(3);

    std::ifstream ifs(receiver_file.c_str());
    if (!ifs.is_open())
      throw(exceptions::msg() << "can't open the receiving file");
    char buf[4096];
    ifs.getline(buf, 4096);
    if (ifs.fail())
      throw(exceptions::msg() << "can't read the receiving fifo");
    ifs.close();
    std::string got = buf;
    if (got != "test")
      throw(exceptions::msg() << "unexpected string received, got: " << got);

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();
  recursive_remove(fifo_config_path);
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
