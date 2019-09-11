/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include "com/centreon/broker/logging/logging.hh"
#include <gtest/gtest.h>
#include <regex.h>
#include <sys/types.h>
#include <cstdio>
#include <fstream>
#include <thread>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker;

// Log messages.
#define MSG1 "my first normal message"
#define MSG2 "my second foobar longer message"
#define MSG3 "my third message is even longer than the second"
#define MSG4 "my fourth messages is finally the longest of all bazqux"
#define MSG5 "my fifth message is shorter"
#define MSG6 "i'm tired of writing message, this is number 6"
#define MSG7 "my seventh message"
#define MSG8 "finally this is the last message for real"

class LoggingManager : public ::testing::Test {
 public:
  void SetUp() override {
    logging::manager::load();
  }

  void TearDown() override {
    logging::manager::unload();
  }

  bool check_line(std::string const& line, std::string const& reg) {
    regex_t r;
    int status;

    status = ::regcomp(&r, reg.c_str(), REG_EXTENDED);

    if (status != 0) {
      char* err = new char[128];
      regerror(status, &r, err, 128);
      std::cout << "cannot compile regex :" << err << std::endl;
      delete err;
      return false;
    }

    status = ::regexec(&r, line.c_str(), 0, nullptr, 0);
    ::regfree(&r);

    return status == 0;
  }

  /**
   *  Write log messages to a backend.
   *
   *  @param[out] b      Backend to write to.
   *  @param[in]  msg_nb Number of messages to write.
   */
  void write_log_messages(logging::backend* b, unsigned int msg_nb = 4) {
    // First message.
    b->log_msg(MSG1 "\n", sizeof(MSG1), logging::config_type, logging::high);
    if (msg_nb <= 1)
      return;

    // Second message.
    b->log_msg(MSG2 "\n", sizeof(MSG2), logging::debug_type, logging::medium);
    if (2 == msg_nb)
      return;

    // Third message.
    b->log_msg(MSG3 "\n", sizeof(MSG3), logging::error_type, logging::low);
    if (3 == msg_nb)
      return;

    // Fourth message.
    b->log_msg(MSG4 "\n", sizeof(MSG4), logging::info_type, logging::high);
    if (4 == msg_nb)
      return;

    // Fifth message.
    b->log_msg(MSG5 "\n", sizeof(MSG5), logging::config_type, logging::low);
    if (5 == msg_nb)
      return;

    b->log_msg(MSG6 "\n", sizeof(MSG6), logging::debug_type, logging::low);
    b->log_msg(MSG7 "\n", sizeof(MSG7), logging::info_type, logging::medium);
    b->log_msg(MSG8 "\n", sizeof(MSG8), logging::error_type, logging::medium);

    return;
  }
};

/**
 *  Check that the logging manager works as expected.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
TEST_F(LoggingManager, Simple) {
  // Log file path.
  std::string file_path{misc::temp_path()};
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Add logging file.
  logging::manager::instance().log_on(
      f,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Write log messages.
  write_log_messages(&f, 4);

  // Check file content.
  std::ifstream infile(file_path);
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]+: +" MSG3 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]+: +" MSG4 "$"}));

  // Remove file.
  std::remove(file_path.c_str());
}

TEST_F(LoggingManager, Multiple) {
  // Log file path.
  std::string file_path1(misc::temp_path());
  std::string file_path2(misc::temp_path());
  std::remove(file_path1.c_str());
  std::remove(file_path2.c_str());

  // Open log file objects.
  logging::file f1(file_path1);
  f1.with_thread_id(false);
  f1.with_timestamp(logging::no_timestamp);
  logging::file f2(file_path2);
  f2.with_thread_id(false);
  f2.with_timestamp(logging::no_timestamp);

  // Add logging files.
  logging::manager::instance().log_on(
      f1,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);
  logging::manager::instance().log_on(
      f2,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Write log messages.
  write_log_messages(&logging::manager::instance(), 4);

  std::ifstream infile(file_path1);
  std::ifstream infile2(file_path2);
  std::string line1;
  std::string line2;

  std::getline(infile, line1);
  ASSERT_TRUE(line1 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG3 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG4 "$"}));

  std::getline(infile2, line2);
  ASSERT_TRUE(line2 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG3 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG4 "$"}));

  // Remove file.
  std::remove(file_path1.c_str());
  std::remove(file_path2.c_str());
}

TEST_F(LoggingManager, Cross) {
  // Log file path.
  std::string file_path1(misc::temp_path());
  std::string file_path2(misc::temp_path());
  std::string file_path3(misc::temp_path());
  std::remove(file_path1.c_str());
  std::remove(file_path2.c_str());
  std::remove(file_path3.c_str());

  // Open log file objects.
  logging::file f1(file_path1);
  f1.with_thread_id(false);
  f1.with_timestamp(logging::no_timestamp);
  logging::file f2(file_path2);
  f2.with_thread_id(false);
  f2.with_timestamp(logging::no_timestamp);
  logging::file f3(file_path3);
  f3.with_thread_id(false);
  f3.with_timestamp(logging::no_timestamp);

  // Add logging files.
  logging::manager::instance().log_on(
      f1, logging::config_type | logging::error_type, logging::high);
  logging::manager::instance().log_on(
      f2, logging::config_type | logging::debug_type, logging::low);
  logging::manager::instance().log_on(f3, logging::info_type, logging::medium);

  // Write log messages.
  write_log_messages(&logging::manager::instance(), 8);

  // Check files content.
  std::ifstream infile(file_path1);
  std::ifstream infile2(file_path2);
  std::ifstream infile3(file_path3);
  std::string line1;
  std::string line2;
  std::string line3;

  std::getline(infile, line1);
  ASSERT_TRUE(line1 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG1 "$"}));

  std::getline(infile2, line2);
  ASSERT_TRUE(line2 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG5 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG6 "$"}));

  std::getline(infile3, line3);
  ASSERT_TRUE(line3 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile3, line3);
  ASSERT_TRUE(check_line(line3, std::string{"^[a-z]+: +" MSG4 "$"}));
  std::getline(infile3, line3);
  ASSERT_TRUE(check_line(line3, std::string{"^[a-z]+: +" MSG7 "$"}));

  // Remove file.
  std::remove(file_path1.c_str());
  std::remove(file_path2.c_str());
  std::remove(file_path3.c_str());
}

TEST_F(LoggingManager, BacklogUnlog) {
  // Log file path.
  std::string file_path1(misc::temp_path());
  std::string file_path2(misc::temp_path());
  std::remove(file_path1.c_str());
  std::remove(file_path2.c_str());

  // Open log file objects.
  logging::file f1(file_path1);
  f1.with_thread_id(false);
  f1.with_timestamp(logging::no_timestamp);
  logging::file f2(file_path2);
  f2.with_thread_id(false);
  f2.with_timestamp(logging::no_timestamp);

  // Add logging files.
  logging::manager::instance().log_on(
      f1,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);
  logging::manager::instance().log_on(
      f2,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Write log messages.
  write_log_messages(&logging::manager::instance(), 4);

  // Stop logging on second backend.
  logging::manager::instance().log_on(f2, 0, logging::none);

  // Write more messages.
  write_log_messages(&logging::manager::instance(), 4);

  std::ifstream infile(file_path1);
  std::ifstream infile2(file_path2);
  std::string line1;
  std::string line2;

  std::getline(infile, line1);
  ASSERT_TRUE(line1 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG3 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG4 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG3 "$"}));
  std::getline(infile, line1);
  ASSERT_TRUE(check_line(line1, std::string{"^[a-z]+: +" MSG4 "$"}));

  std::getline(infile2, line2);
  ASSERT_TRUE(line2 == "Centreon Broker " CENTREON_BROKER_VERSION
                       " log file opened");
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG1 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG2 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG3 "$"}));
  std::getline(infile2, line2);
  ASSERT_TRUE(check_line(line2, std::string{"^[a-z]+: +" MSG4 "$"}));

  // Remove file.
  std::remove(file_path1.c_str());
  std::remove(file_path2.c_str());
}

TEST_F(LoggingManager, Everything) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(true);
  f.with_timestamp(logging::second_timestamp);

  // Write log messages.
  write_log_messages(&f);

  std::ifstream infile(file_path);
  std::string line;

  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line,
      std::string{"^\\[[0-9]*\\] \\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG1 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line,
      std::string{"^\\[[0-9]*\\] \\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG2 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line,
      std::string{"^\\[[0-9]*\\] \\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG3 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line,
      std::string{"^\\[[0-9]*\\] \\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG4 "$"}));

  // Remove file.
  std::remove(file_path.c_str());
}

#define MAX_SIZE 2000000ull

TEST_F(LoggingManager, MaxSize) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::string backup_file_path(file_path);
  backup_file_path.append(".old");
  std::remove(file_path.c_str());
  std::remove(backup_file_path.c_str());

  // Open log file object.
  logging::file f(file_path, MAX_SIZE);
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Write log messages.
  for (unsigned int i = 0; i < 200000; ++i)
    write_log_messages(&f);

  std::ifstream ifs(file_path, std::ios::binary);
  ASSERT_TRUE(ifs.good());
  std::streampos begin{ifs.tellg()};
  ifs.seekg(0, std::ios::end);
  std::streampos end{ifs.tellg()};
  std::streampos fsize1(end - begin);

  std::ifstream ifs2(file_path, std::ios::binary);
  ASSERT_TRUE(ifs2.good());
  std::streampos begin2{ifs2.tellg()};
  ifs2.seekg(0, std::ios::end);
  std::streampos end2{ifs2.tellg()};
  std::streampos fsize2(end2 - begin2);

  ASSERT_TRUE(fsize1 >= 0);
  ASSERT_TRUE(fsize2 >= 0);
  ASSERT_TRUE(fsize1 <= static_cast<int64_t>(MAX_SIZE));
  ASSERT_TRUE(fsize2 <= static_cast<int64_t>(MAX_SIZE));

  // Remove files.
  std::remove(file_path.c_str());
  std::remove(backup_file_path.c_str());
}

TEST_F(LoggingManager, Nothing) {
  // Log file path.
  std::string file_path{misc::temp_path()};
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f{file_path};
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Write log messages.
  write_log_messages(&f);
  // Check file content.
  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]*: *" MSG1 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]*: *" MSG2 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]*: *" MSG3 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^[a-z]*: *" MSG4 "$"}));

  // Remove file.
  std::remove(file_path.c_str());
}

TEST_F(LoggingManager, ThreadId) {
  // Log file path.
  std::string file_path{misc::temp_path()};
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f{file_path};
  f.with_thread_id(true);
  f.with_timestamp(logging::no_timestamp);

  // Write log messages.
  write_log_messages(&f);

  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line, std::string{"^\\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG1 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line, std::string{"^\\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG2 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line, std::string{"^\\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG3 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line, std::string{"^\\[0x[0-9abcdef]*\\] [a-zA-Z]*: *" MSG4 "$"}));

  // Remove file.
  std::remove(file_path.c_str());
}

TEST_F(LoggingManager, Timestamp) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(false);
  f.with_timestamp(logging::second_timestamp);

  // Write log messages.
  write_log_messages(&f);

  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(
      check_line(line, std::string{"^\\[[0-9]*\\] [a-zA-Z]*: *" MSG1 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(
      check_line(line, std::string{"^\\[[0-9]*\\] [a-zA-Z]*: *" MSG2 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(
      check_line(line, std::string{"^\\[[0-9]*\\] [a-zA-Z]*: *" MSG3 "$"}));
  std::getline(infile, line);
  ASSERT_TRUE(
      check_line(line, std::string{"^\\[[0-9]*\\] [a-zA-Z]*: *" MSG4 "$"}));
}

TEST_F(LoggingManager, Copied) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Add logging object.
  logging::manager::instance().log_on(
      f,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Create and destroy temp_logger objects.
  {
    logging::temp_logger tl1(logging::config_type, logging::high, true);
    logging::temp_logger tl2(tl1);
  }

  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^config:  $"}));

  // Remove file.
  std::remove(file_path.c_str());
}

TEST_F(LoggingManager, Disabled) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Add logging object.
  logging::manager::instance().log_on(
      f,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Create and destroy temp_logger.
  {
    logging::temp_logger tl(logging::config_type, logging::high, false);
    tl << true << 42 << "a random string";
  }

  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  // Remove file.
  std::remove(file_path.c_str());
}

TEST_F(LoggingManager, Enabled) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Add logging object.
  logging::manager::instance().log_on(
      f,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Create and destroy temp_logger.
  { logging::temp_logger tl(logging::config_type, logging::high, true); }

  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(line, std::string{"^config:  $"}));

  // Remove file.
  std::remove(file_path.c_str());
}

TEST_F(LoggingManager, Insertion) {
  // Log file path.
  std::string file_path(misc::temp_path());
  std::remove(file_path.c_str());

  // Open log file object.
  logging::file f(file_path);
  f.with_thread_id(false);
  f.with_timestamp(logging::no_timestamp);

  // Add logging object.
  logging::manager::instance().log_on(
      f,
      logging::config_type | logging::debug_type | logging::error_type |
          logging::info_type,
      logging::low);

  // Create and destroy temp_logger objects.
  {
    logging::temp_logger tl(logging::config_type, logging::high, true);
    tl << true << 42 << "my own random string" << -789527845245ll;
  }

  std::ifstream infile{file_path};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");
  std::getline(infile, line);
  ASSERT_TRUE(check_line(
      line, std::string{"^config:  true42my own random string-789527845245$"}));

  // Remove file.
  std::remove(file_path.c_str());
}

#define MESSAGE "0123456789abcdefghijklmnopqrstuvwxyz"
#define WRITE_COUNT 200

/**
 *  Log some messages.
 */
static void log_messages() {
  for (unsigned int i = 0; i < WRITE_COUNT; ++i)
    logging::error(logging::high) << MESSAGE;
  return;
}

/**
 *  Check that logging is thread-safe.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
TEST_F(LoggingManager, Concurrent) {
  // Build filename.
  std::string filename(misc::temp_path());

  // Remove old file.
  std::remove(filename.c_str());

  // Initialize file backend.
  logging::file::with_timestamp(logging::second_timestamp);
  logging::file backend(filename);

  // Add backend to logging list.
  logging::manager::instance().log_on(backend);

  // launch threads.
  std::vector<std::thread> pool;
  for (unsigned int i = 0; i < 50; ++i)
    pool.push_back(std::thread(log_messages));

  // Wait for tasks to finish.
  for (unsigned int i = 0; i < 50; ++i)
    pool[i].join();

  // Remove backend from logging.
  logging::manager::instance().log_on(backend, 0, logging::none);

  std::ifstream infile{filename};
  std::string line;
  std::getline(infile, line);
  ASSERT_TRUE(line == "Centreon Broker " CENTREON_BROKER_VERSION
                      " log file opened");

  while (std::getline(infile, line))
    ASSERT_TRUE(check_line(line, std::string{".*" MESSAGE ".*"}));

  // Remove temp file.
  std::remove(filename.c_str());
}
