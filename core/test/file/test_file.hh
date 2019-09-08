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
#ifndef TEST_FILE_HH
#define TEST_FILE_HH

#include <map>
#include <string>
#include "com/centreon/broker/file/fs_file.hh"

class test_file : public com::centreon::broker::file::fs_file {
 public:
  test_file(std::string* content);
  ~test_file();
  void close();
  long read(void* buffer, long max_size);
  void seek(long offset,
            com::centreon::broker::file::fs_file::seek_whence whence);
  long tell();
  long write(void const* buffer, long size);

 private:
  test_file(test_file const& other);
  test_file& operator=(test_file const& other);

  std::string* _content;
  int _pos;
};

class test_file_factory : public com::centreon::broker::file::fs_file_factory {
 public:
  test_file_factory();
  ~test_file_factory();
  std::string& get(std::string const& path);
  com::centreon::broker::file::fs_file* new_fs_file(
      std::string const& path,
      com::centreon::broker::file::fs_file::open_mode mode);

 private:
  test_file_factory(test_file_factory& other);
  test_file_factory& operator=(test_file_factory& other);

  std::map<std::string, std::string> _files;
};

#endif  // !TEST_FILE_HH
