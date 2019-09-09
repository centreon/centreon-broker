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
#include "com/centreon/broker/file/cfile.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

class FileSplitterSplit : public ::testing::Test {
 public:
  void SetUp() override {
    logging::manager::load();

    _path = "/tmp/queue";
    {
      std::list<std::string> parts{misc::filesystem::dir_content_with_filter("/tmp/", "queue*")};
      for (std::string const& f : parts)
        std::remove(f.c_str());
    }
    _file.reset(new file::splitter(
        _path, file::fs_file::open_read_write_truncate, 10008, true));
    char buffer[10];
    for (int i(0); i < 10; ++i)
      buffer[i] = i;
    for (int i(0); i < 10001; ++i)
      _file->write(buffer, sizeof(buffer));
    _file->flush();
  }

  void TearDown() override { logging::manager::unload(); }

 protected:
  std::unique_ptr<file::splitter> _file;
  std::string _path;
};

// Given a splitter object configured with a max_size of 10008
// When write() is called 10001 times with 10 bytes of data
// Then ten files are created
// And all files but the last are 10008 bytes long
// And the last file is 18 bytes long
TEST_F(FileSplitterSplit, MultipleFilesCreated) {
  // Then
  ASSERT_EQ(misc::filesystem::file_size(_path), 10008u);
  for (int i(1); i < 10; ++i) {
    std::ostringstream oss;
    oss << _path << i;
    ASSERT_EQ(misc::filesystem::file_size(oss.str()), 10008u);
  }
  std::string last_file(_path);
  last_file.append("10");
  ASSERT_EQ(misc::filesystem::file_size(last_file), 18u);
}

// Given a splitter object configured with a max_size of 10008
// And write() was called 10001 times with 10 bytes of data
// When I read 10 times for a maximum of 10001 bytes
// Then every time 10000 bytes are read
// And the last time 10 bytes are read
TEST_F(FileSplitterSplit, EntireFilesReadBack) {
  // When
  char buffer[10001];
  for (int i(0); i < 10; ++i) {
    // Then
    long bytes_read(_file->read(buffer, sizeof(buffer)));
    ASSERT_EQ(bytes_read, 10000);
  }
  long bytes_read(_file->read(buffer, sizeof(buffer)));
  ASSERT_EQ(bytes_read, 10);
}
