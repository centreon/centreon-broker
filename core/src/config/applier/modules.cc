/*
** Copyright 2011-2013, 2021 Centreon
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

#include "com/centreon/broker/config/applier/modules.hh"
#include <cstdlib>
#include <memory>
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/modules/handle.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker::config::applier;

/**
 *  Apply a module configuration.
 *
 *  @param[in] module_list Module list.
 *  @param[in] module_dir  Module directory.
 *  @param[in] arg         Module argument.
 */
void modules::apply(std::list<std::string> const& module_list,
                    std::string const& module_dir,
                    void const* arg) {
  // Load modules.
  for (std::string const& m : module_list) {
    logging::config(logging::high)
        << "module applier: loading module '" << m << "'";
    load_file(m, arg);
  }
  if (!module_dir.empty()) {
    logging::config(logging::high)
        << "module applier: loading directory '" << module_dir << "'";
    load_dir(module_dir, arg);
  } else
    logging::debug(logging::high) << "module applier: no directory defined";
}

/**
 *  Get iterator to the first module.
 *
 *  @return Iterator to the first module.
 */
modules::iterator modules::begin() {
  return _handles.begin();
}

/**
 *  Unload modules.
 */
void modules::discard() {
  auto rit = _handles.rbegin();
  while (rit != _handles.rend()) {
    _handles.erase(rit->first);
    rit = _handles.rbegin();
  }
}

/**
 *  Get last iterator of the module list.
 *
 *  @return Last iterator of the module list.
 */
modules::iterator modules::end() {
  return _handles.end();
}

std::mutex& modules::module_mutex() {
  return _m_modules;
}

/**
 *  Load a plugin.
 *
 *  @param[in] filename File name.
 *  @param[in] arg      Module argument.
 */
void modules::load_file(const std::string& filename, const void* arg) {
  auto it = _handles.find(filename);
  if (it == _handles.end())
    _handles.emplace(filename, std::make_shared<handle>(filename, arg));
  else {
    log_v2::core()->info(
        "modules: attempt to load '{}' which is already loaded", filename);
    it->second->update(arg);
  }
}

/**
 * @brief Load a directory containing plugins.
 *
 * @param dirname Directory name.
 * @param arg     Module argument.
 */
void modules::load_dir(const std::string& dirname, const void* arg) {
  // Debug message.
  log_v2::core()->debug("modules: loading directory '{}'", dirname);

  // Set directory browsing parameters.
  std::list<std::string> list =
      misc::filesystem::dir_content_with_filter(dirname, "*.so");
  list.sort();

  for (auto& l : list) {
    try {
      load_file(l, arg);
    } catch (const msg_fmt& e) {
      logging::error(logging::high) << e.what();
    }
  }

  // Ending log message.
  log_v2::core()->debug("modules: finished loading directory '{}'", dirname);
}
