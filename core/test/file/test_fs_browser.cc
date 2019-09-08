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
#include "test_fs_browser.hh"

using namespace com::centreon::broker;

test_fs_browser::test_fs_browser() {}

test_fs_browser::~test_fs_browser() {}

void test_fs_browser::add_result(file::fs_browser::entry_list const& result) {
  _results.push_back(result);
  return;
}

file::fs_browser::entry_list test_fs_browser::read_directory(
    std::string const& path,
    std::string const& filters) {
  (void)path;
  (void)filters;
  file::fs_browser::entry_list retval;
  if (!_results.empty()) {
    retval = _results.front();
    _results.pop_front();
  }
  return (retval);
}

std::list<std::string> const& test_fs_browser::get_removed() const {
  return (_removed);
}

void test_fs_browser::remove(std::string const& path) {
  _removed.push_back(path);
  return;
}
