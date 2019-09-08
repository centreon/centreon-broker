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
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/broker_extcmd.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define RECEIVER_DIRECTORY "dir_dumper_receiver"
#define SENDER_DIRECTORY "dir_dumper_sender"
#define SUB_DIRECTORY "dir_dumper_subdirectory"
#define SENT_FILENAME "dir_sent_file"
#define SENT_SUBDIR_FILENAME "dir_sent_subdir_file"

void check_file_content(std::string const& filename,
                        std::string const& content) {
  // Check for file existence.
  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open())
    throw(exceptions::msg() << "can't open the file: " << filename);
  char buf[4096];
  ifs.getline(buf, 4096);
  if (ifs.fail())
    throw(exceptions::msg() << "can't read the file: " << filename);
  ifs.close();
  std::string got = buf;
  if (got != content)
    throw(exceptions::msg() << "unexpected string received, got: " << got);
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
    file.set_template(PROJECT_SOURCE_DIR "/test/cfg/directory_dumper.xml.in");
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
      throw(exceptions::msg()
            << "can't start the process to write into the sent file "
            << process->errorString());
    QProcess* process2 = new QProcess();
    command = "./util_write_into_file \"test2\n\" " + sent_subdir_file;
    process2->start(QString::fromStdString(command));
    if (process2->waitForStarted() == false)
      throw(exceptions::msg()
            << "can't start the process to write into the sent subdir file "
            << process2->errorString());

    broker_extcmd exc;
    exc.set_file(dumper_command_file);
    exc.execute("EXECUTE;84;directorydumper-read;DUMP_DIR");

    sleep_for(3);

    check_file_content(expected_file, "test");
    check_file_content(expected_subdir_file, "test2");

    // Remove file.
    ::remove(sent_file.c_str());
    exc.execute("EXECUTE;84;directorydumper-read;DUMP_DIR");
    sleep_for(3);

    // Check for file removal
    std::ifstream ifs;
    ifs.open(expected_file.c_str());
    if (ifs.is_open())
      throw(exceptions::msg() << "file '" << expected_file << "' not deleted");

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
  recursive_remove(receiver_dir);
  recursive_remove(sender_dir);
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
