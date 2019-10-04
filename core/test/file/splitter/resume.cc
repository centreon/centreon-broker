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
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/cfile.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/misc/filesystem.hh"

using namespace com::centreon::broker;

class FileSplitterResume : public ::testing::Test {
 public:
  void SetUp() override {

    std::list<std::string> lst{
        misc::filesystem::dir_content_with_filter("/tmp/", "queue*")};
    for (std::string const& f : lst)
      std::remove(f.c_str());
    _path = "/tmp/queue";

    // Create 7 files.
    std::unique_ptr<file::fs_file> f;
    char buffer[10008];
    for (int i(2); i < 10; ++i) {
      memset(buffer, i, sizeof(buffer));
      std::ostringstream oss;
      oss << _path << i;
      f.reset(new file::cfile(oss.str(), file::fs_file::open_read_write_truncate));
      f->write(buffer, sizeof(buffer));
    }

    // Create the last file.
    std::string last_file(_path);
    last_file.append("10");
    f.reset(new file::cfile(last_file, file::fs_file::open_read_write_truncate));
    memset(buffer, 10, sizeof(buffer));
    f->write(buffer, 108);
    f.reset();

    // Create new splitter.
    _file.reset(new file::splitter(_path,
                                   file::fs_file::open_read_write_truncate,
                                   10000, true));
  }

 protected:
  std::unique_ptr<file::splitter> _file;
  std::string _path;
};

// Given existing file parts, from 2 to 10
// And a splitter object
// When read() is called
// Then data is read from file part 2
TEST_F(FileSplitterResume, ResumeRead) {
  // When
  char buffer[10000];
  long bytes_read(_file->read(buffer, sizeof(buffer)));

  // Then
  ASSERT_EQ(bytes_read, 10000);
  for (uint32_t i(0); i < sizeof(buffer); ++i)
    ASSERT_EQ(buffer[i], 2);
}

// Given existing file parts, from 2 to 10
// And a splitter object
// When write() is called
// Then data is appended to file part 10
TEST_F(FileSplitterResume, ResumeWrite) {
  // When
  char buffer[2000];
  memset(buffer, 42, sizeof(buffer));
  _file->write(buffer, sizeof(buffer));

  // Then
  std::string last_file(_path);
  last_file.append("10");
  _file->flush();
  ASSERT_EQ(misc::filesystem::file_size(last_file), 108u + 2000u);
}

// Given existing files parts, from 2 to 10
// And a splitter object
// When read() is called
// Then files are deleted when they are entirely read back
TEST_F(FileSplitterResume, AutoDelete) {
  // When
  char buffer[10000];
  for (int i(2); i <= 10; ++i)
    _file->read(buffer, sizeof(buffer));
  ASSERT_THROW(_file->read(buffer, 1), exceptions::shutdown);

  // Then
  for (int i(2); i <= 10; ++i) {
    std::ostringstream oss;
    oss << _path << i;
    ASSERT_FALSE(misc::filesystem::file_exists(oss.str()));
  }
}
