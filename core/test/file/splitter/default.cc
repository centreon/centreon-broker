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
#include "../test_file.hh"
#include "../test_fs_browser.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

class FileSplitterDefault : public ::testing::Test {
 public:
  void SetUp() override {
    logging::manager::load();
    _path = "queue";
    _file_factory = new test_file_factory();
    _fs_browser = new test_fs_browser();
    _file.reset(new file::splitter(_path,
                                   file::fs_file::open_read_write_truncate,
                                   _file_factory, _fs_browser, 10000, true));
  }

  void TearDown() override { logging::manager::unload(); }

 protected:
  std::unique_ptr<file::splitter> _file;
  test_file_factory* _file_factory;
  test_fs_browser* _fs_browser;
  std::string _path;
};

// Given a splitter object
// When it is created
// Then a file is created with a size of 8 bytes
TEST_F(FileSplitterDefault, DefaultFile) {
  // Then
  ASSERT_EQ(_file_factory->get(_path).size(), 8u);
}

// Given a splitter object
// When write() is called with 1000 bytes
// Then its return value is 1000
TEST_F(FileSplitterDefault, WriteReturnsNumberOfBytes) {
  // When
  char buffer[1000];
  memset(buffer, 42, sizeof(buffer));
  long wb(_file->write(buffer, sizeof(buffer)));
  ASSERT_EQ(wb, static_cast<long>(sizeof(buffer)));
}

// Given a splitter object
// When read() is called
// Then an io::exceptions::shutdown exception is thrown
// And the on-disk file is deleted
TEST_F(FileSplitterDefault, FirstReadNoDataAndRemove) {
  // Then
  char buffer[10];
  ASSERT_THROW(_file->read(buffer, sizeof(buffer)), exceptions::shutdown);
  std::list<std::string> removed;
  removed.push_back(_path);
  ASSERT_EQ(_fs_browser->get_removed(), removed);
}
