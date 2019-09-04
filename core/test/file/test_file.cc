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
#include <cstring>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "test_file.hh"

using namespace com::centreon::broker;

test_file::test_file(std::string* content)
  : _content(content), _pos(0) {}

test_file::~test_file() {}

void test_file::close() {}

long test_file::read(void* buffer, long max_size) {
  long size(_content->size() - _pos);
  if (!size)
    throw (exceptions::shutdown() << "end of file");
  else if (max_size < size)
    size = max_size;
  memcpy(buffer, _content->data() + _pos, size);
  _pos += size;
  return (size);
}

void test_file::seek(long offset, file::fs_file::seek_whence whence) {
  switch (whence) {
   case seek_start:
    _pos = offset;
    break ;
   case seek_current:
    _pos += offset;
    break ;
   case seek_end:
    _pos = _content->size() - 1 + offset;
    break;
  };
  return ;
}

long test_file::tell() {
  return (_pos);
}

long test_file::write(void const* buffer, long size) {
  _content->append(static_cast<char const*>(buffer), size);
  return (size);
}

test_file_factory::test_file_factory() {}

test_file_factory::~test_file_factory() {}

std::string& test_file_factory::get(std::string const& path) {
  std::map<std::string, std::string>::iterator
    it(_files.find(path));
  if (it == _files.end())
    throw (exceptions::msg() << "file " << path << " does not exist");
  return (it->second);
}

file::fs_file* test_file_factory::new_fs_file(
                                    std::string const& path,
                                    file::fs_file::open_mode mode) {
  (void)mode;
  return (new test_file(&_files[path]));
}
