/*
** Copyright 2012-2015 Centreon
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

#include "test/file.hh"
#include <cstdio>
#include <fstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**
 *  Default constructor.
 */
file::file() {
  _variables["PROJECT_SOURCE_DIR"] = PROJECT_SOURCE_DIR;
  _variables["CBD_PATH"] = CBD_PATH;
  _variables["CBMOD_PATH"] = CBMOD_PATH;
  _variables["MY_PLUGIN_PATH"] = MY_PLUGIN_PATH;
  _variables["MY_PLUGIN_BAM_PATH"] = MY_PLUGIN_BAM_PATH;
  _variables["BENCH_GENERATE_RRD_MOD_PATH"] = BENCH_GENERATE_RRD_MOD_PATH;
  _variables["MONITORING_ENGINE"] = MONITORING_ENGINE;
  _variables["MONITORING_ENGINE_ADDITIONAL"] = MONITORING_ENGINE_ADDITIONAL;
  _variables["MONITORING_ENGINE_INTERVAL_LENGTH"] =
      MONITORING_ENGINE_INTERVAL_LENGTH_STR;
  _variables["MONITORING_ENGINE_INTERVAL_LENGTH_STR"] =
      MONITORING_ENGINE_INTERVAL_LENGTH_STR;
  _variables["DB_TYPE"] = DB_TYPE;
  _variables["DB_HOST"] = DB_HOST;
  _variables["DB_PORT"] = DB_PORT;
  _variables["DB_USER"] = DB_USER;
  _variables["DB_PASSWORD"] = DB_PASSWORD;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
file::file(file const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
file::~file() {
  close();
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
file& file::operator=(file const& other) {
  if (this != &other) {
    close();
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Close a generated file. This effectively removes the generated file.
 */
void file::close() {
  if (!_target_file.empty()) {
    ::remove(_target_file.c_str());
    _target_file.clear();
  }
  return;
}

/**
 *  Generate a test file from a template.
 *
 *  @return The path to the generated file.
 */
std::string const& file::generate() {
  // Close old file.
  close();

  // Read base file.
  std::string content;
  {
    std::ifstream ifs;
    ifs.open(_base_file.c_str());
    if (!ifs.good())
      throw(exceptions::msg()
            << "could not open base file '" << _base_file << "'");
    char buffer[4096];
    while (ifs.good()) {
      ifs.read(buffer, sizeof(buffer));
      content.append(buffer, ifs.gcount());
    }
    ifs.close();
  }

  // Replace variables.
  size_t start(content.find_first_of('@', 0));
  while (start != std::string::npos) {
    size_t end(content.find_first_of('@', start + 1));
    if (std::string::npos == end)
      throw(exceptions::msg()
            << "non-terminated variable (\"@VAR@\") in base file '"
            << _base_file << "' at offset " << start << " "
            << content.substr(start));
    std::string var(content.substr(start + 1, end - start - 1));
    std::map<std::string, std::string>::const_iterator it(_variables.find(var));
    if (it != _variables.end()) {
      content.replace(start, end - start + 1, it->second);
      start += it->second.size();
    } else {
      content.erase(start, end - start + 1);
    }
    start = content.find_first_of('@', start);
  }

  // Write target file.
  _target_file = misc::temp_path();
  std::ofstream ofs;
  ofs.open(_target_file.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (!ofs.good()) {
    ofs.close();
    close();
    throw(exceptions::msg() << "could not open target file");
  }
  ofs.write(content.c_str(), content.size());
  if (!ofs.good()) {
    ofs.close();
    close();
    throw(exceptions::msg() << "could not write content to target file");
  }
  ofs.close();
  return (_target_file);
}

/**
 *  Set a variable.
 *
 *  @param[in] variable  Variable name.
 *  @param[in] value     Value.
 */
void file::set(std::string const& variable, std::string const& value) {
  _variables[variable] = value;
  return;
}

/**
 *  Set the template file.
 *
 *  @param[in] base_file  Base file from which we will generate the test
 *                        file.
 */
void file::set_template(std::string const& base_file) {
  _base_file = base_file;
  return;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void file::_internal_copy(file const& other) {
  _base_file = other._base_file;
  _variables = other._variables;
  return;
}
