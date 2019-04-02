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
#include "com/centreon/broker/file/splitter.hh"
#include "../test_file.hh"
#include "../test_fs_browser.hh"

using namespace com::centreon::broker;

class  FileSplitterMoreThanMaxSize : public ::testing::Test {
 public:
  void SetUp() {
    _path = "/var/lib/centreon-broker/queue";
    _file_factory = new test_file_factory();
    _fs_browser = new test_fs_browser();
    _file.reset(new file::splitter(
                            _path,
                            file::fs_file::open_read_write_truncate,
                            _file_factory,
                            _fs_browser,
                            10000,
                            true));
    return ;
  }

 protected:
  std::unique_ptr<file::splitter> _file;
  test_file_factory*            _file_factory;
  test_fs_browser*              _fs_browser;
  std::string                   _path;
};

// Given a splitter object configured with a max_size of 10000
// And write() was called with 1 byte of data
// When write() is called with 10001 bytes of data
// Then the first file has 1 byte of data
// And the second file is longer than max_size
TEST_F(FileSplitterMoreThanMaxSize, MoreThanMaxSizeToNextFile) {
  // Given
  char buffer[10001];
  memset(buffer, 0, sizeof(buffer));
  _file->write(buffer, 1);

  // When
  _file->write(buffer, sizeof(buffer));

  // Then
  std::string first_file(_path);
  ASSERT_EQ(_file_factory->get(first_file).size(), 9u);
  std::string second_file(_path);
  second_file.append("1");
  ASSERT_EQ(_file_factory->get(second_file).size(), sizeof(buffer) + 8);
}

// Given a splitter object configured with a max_size of 10000
// And write() was called with 1 byte of data
// And write() was called with 10001 bytes of data
// When read() is called
// Then 1 byte is read
TEST_F(FileSplitterMoreThanMaxSize, ReadBeforeMoreThanMaxSize) {
  // Given
  char buffer[10001];
  memset(buffer, 0, sizeof(buffer));
  _file->write(buffer, 1);
  _file->write(buffer, sizeof(buffer));

  // When
  long read_bytes(_file->read(buffer, sizeof(buffer)));

  // Then
  ASSERT_EQ(read_bytes, 1);
}

// Given a splitter object configured with a max_size of 10000
// And write() was called with 1 byte of data
// And write() was called with 10001 bytes of data
// And read() is called once
// When read() is called again with a 10010 bytes buffer
// Then 10001 bytes are read
TEST_F(FileSplitterMoreThanMaxSize, ReadMoreThanMaxSize) {
  // Given
  char buffer[10010];
  for (unsigned int i(0); i < sizeof(buffer); ++i)
    buffer[i] = i % 128;
  _file->write(buffer, 1);
  _file->write(buffer, 10001);
  _file->read(buffer, sizeof(buffer));

  // When
  memset(buffer, 0, sizeof(buffer));
  long read_bytes(_file->read(buffer, sizeof(buffer)));

  // Then
  ASSERT_EQ(read_bytes, 10001);
  for (int i(0); i < read_bytes; ++i)
    ASSERT_EQ(buffer[i], i % 128);
}
