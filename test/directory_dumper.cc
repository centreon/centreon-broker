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
#include "external_command.hh"
#include "vars.hh"
#include "cbd.hh"
#include "generate.hh"
#include "misc.hh"
#include "vars.hh"

using namespace com::centreon::broker;

#define RECEIVER_DIRECTORY "dir_dumper_receiver"
#define SENDER_DIRECTORY "dir_dumper_sender"
#define SUB_DIRECTORY "dir_dumper_subdirectory"
#define SENT_FILENAME "dir_sent_file"
#define SENT_SUBDIR_FILENAME "dir_sent_subdir_file"

void check_file_content(
       std::string const& filename,
       std::string const& content) {
  // Check for file existence.
  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open())
    throw (exceptions::msg()
           << "can't open the file: " << filename);
  char buf[4096];
  ifs.getline(buf, 4096);
  if (ifs.fail())
    throw (exceptions::msg()
           << "can't read the file: " << filename);
  ifs.close();
  std::string got = buf;
  if (got != content)
    throw (exceptions::msg()
           << "unexpected string received, got: " << got);
}

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
  std::string expected_subdir_file =
    receiver_dir + "/" + SUB_DIRECTORY + "/" + SENT_SUBDIR_FILENAME;
  std::string sender_dir = fifo_config_path + "/" + SENDER_DIRECTORY;
  std::string subdirectory = sender_dir + "/" + SUB_DIRECTORY;
  std::string sent_file = sender_dir + "/" + SENT_FILENAME;
  std::string sent_subdir_file = subdirectory + "/" + SENT_SUBDIR_FILENAME;
  std::string dumper_command_file = ::tmpnam(NULL);
  cbd broker;

  std::cout << "receiver directory: " << receiver_dir << std::endl;
  std::cout << "sender directory: " << sender_dir << std::endl;

  try {
    ::mkdir(fifo_config_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    ::mkdir(receiver_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    ::mkdir(sender_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    ::mkdir(subdirectory.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    // Create the config influxdb xml file.
    test_file file;
    file.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/directory_dumper.xml.in");
    file.set("SENDER_DIRECTORY", sender_dir);
    file.set("RECEIVER_DIRECTORY", receiver_dir);
    file.set("DUMPER_COMMAND_FILE", dumper_command_file);
    std::string config_file = file.generate();

    broker.set_config_file(config_file);
    broker.start();

    sleep_for(3);

    QProcess* process = new QProcess();
    std::string command = "./util_write_into_file \"test\n\" " + sent_file;
    process->start(QString::fromStdString(command));
    if (process->waitForStarted() == false)
      throw (exceptions::msg()
             << "can't start the process to write into the sent file "
             << process->errorString());
    QProcess* process2 = new QProcess();
    command = "./util_write_into_file \"test2\n\" " + sent_subdir_file;
    process2->start(QString::fromStdString(command));
    if (process2->waitForStarted() == false)
      throw (exceptions::msg()
             << "can't start the process to write into the sent subdir file "
             << process2->errorString());

    external_command exc;
    exc.set_file(dumper_command_file);
    exc.execute("DUMP_DIR;dir_dumper_input");

    sleep_for(3);

    check_file_content(expected_file, "test");
    check_file_content(expected_subdir_file, "test2");

    // Remove file.
    ::remove(sent_file.c_str());
    exc.execute("DUMP_DIR;dir_dumper_input");
    sleep_for(3);

    // Check for file removal
    std::ifstream ifs;
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
