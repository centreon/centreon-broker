/*
** Copyright 2017 Centreon
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

#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "../test_file.hh"
#include "../test_fs_browser.hh"

using namespace com::centreon::broker;

class FileSplitterResume : public ::testing::Test {
 public:
  void SetUp() {
    logging::manager::load();

    _path = "/var/lib/centreon-broker/queue";
    _fs_browser = new test_fs_browser();
    _file_factory = new test_file_factory();

    // Create 7 files.
    std::unique_ptr<file::fs_file> f;
    char buffer[10008];
    for (int i(2); i < 10; ++i) {
      memset(buffer, i, sizeof(buffer));
      std::ostringstream oss;
      oss << _path << i;
      f.reset(_file_factory->new_fs_file(
                               oss.str(),
                               file::fs_file::open_read_write_truncate));
      f->write(buffer, sizeof(buffer));
    }

    // Create the last file.
    std::string last_file(_path);
    last_file.append("10");
    f.reset(_file_factory->new_fs_file(
                             last_file,
                             file::fs_file::open_read_write_truncate));
    memset(buffer, 10, sizeof(buffer));
    f->write(buffer, 108);
    f.reset();

    // Set entries of FS browser.
    file::fs_browser::entry_list list;
    for (int i(2); i <= 10; ++i) {
      std::ostringstream oss;
      oss << "queue" << i;
      list.push_back(oss.str());
    }
    _fs_browser->add_result(list);

    // Create new splitter.
    _file.reset(new file::splitter(
                            _path,
                            file::fs_file::open_read_write_truncate,
                            _file_factory,
                            _fs_browser,
                            10000,
                            true));
  }

  void TearDown() {
      logging::manager::unload();
  };

 protected:
  std::unique_ptr<file::splitter> _file;
  test_file_factory*            _file_factory;
  test_fs_browser*              _fs_browser;
  std::string                   _path;
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
  for (unsigned int i(0); i < sizeof(buffer); ++i)
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
  ASSERT_EQ(_file_factory->get(last_file).size(), 108u + 2000u);
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
  std::list<std::string> removed;
  for (int i(2); i <= 10; ++i) {
    std::ostringstream oss;
    oss << _path << i;
    removed.push_back(oss.str());
  }
  ASSERT_EQ(removed, _fs_browser->get_removed());
}
