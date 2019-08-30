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

#include <QDir>
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

// We need a file on which we do not have access. Here the file does not
// exist but is in a forbidden directory
#define FILE_WITH_BAD_PERMISSION "/root/test-permission-denied"

class FileSplitterPermissionDenied : public ::testing::Test {

 public:
  void SetUp() {
    logging::manager::load();

    _path = FILE_WITH_BAD_PERMISSION;
    _file_factory.reset(new splitter_factory());

    return ;
  }

  void TearDown() {
    logging::manager::unload();
  }

 protected:
  std::unique_ptr<splitter_factory>  _file_factory;
  std::string                   _path;

};

// Given a splitter factory
// When we create a splitter to a file with insufficient access
// Then the creation does not crash
TEST_F(FileSplitterPermissionDenied, DefaultFile) {
  ASSERT_THROW(
    _file_factory->new_cfile_splitter(
                          _path,
                          file::fs_file::open_read_write_truncate,
                          10000,
                          true),
    exceptions::msg);
}
