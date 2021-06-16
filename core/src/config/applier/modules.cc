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
void modules::apply(const std::list<std::string>& module_list,
                    const std::string& module_dir,
                    const void* arg) {
  // Load modules.
  for (const std::string& m : module_list) {
    log_v2::config()->info("module applier: loading module '{}'", m);
    load_file(m, arg);
  }
  if (!module_dir.empty()) {
    log_v2::config()->info("module applier: loading directory '{}'",
                           module_dir);
    load_dir(module_dir, arg);
  } else
    log_v2::config()->debug("module applier: no directory defined");
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
 * @brief Check the shared library passed with its filename and handler.
 *
 * @param name File name of the module.
 * @param h Handler of the module.
 *
 * @return a boolean True on success.
 */
bool modules::_check_module(const std::string& name, void* h) noexcept {
  // Find init symbol.
  log_v2::core()->debug(
      "modules: checking initialization routine (symbol '{}') in '{}'",
      handle::initialization, name);
  void* init = dlsym(h, handle::initialization);

  if (!init) {
    log_v2::core()->error(
        "modules: could not find initialization routine in '{}' (not a "
        "Centreon Broker module ?): {}",
        name, dlerror());
    return false;
  }

  // Find deinit symbol.
  log_v2::core()->debug(
      "modules: checking deinitialization routine (symbol '{}') in '{}'",
      handle::deinitialization, name);
  void* deinit = dlsym(h, handle::deinitialization);

  if (!deinit) {
    log_v2::core()->error(
        "modules: could not find deinitialization routine in '{}' (not a "
        "Centreon Broker module ?): {}",
        name, dlerror());
    return false;
  }

  // Find deinit symbol.
  log_v2::core()->debug(
      "modules: checking updatization routine (symbol '{}') in '{}'",
      handle::updatization, name);
  void* update = dlsym(h, handle::updatization);

  if (!update) {
    log_v2::core()->error(
        "modules: could not find updatization routine in '{}' (not a Centreon "
        "Broker module ?): {}",
        name, dlerror());
    return false;
  }

  // Find version symbol.
  log_v2::core()->debug(
      "modules: checking module version (symbol '{}') in '{}'",
      handle::versionning, name);

  char const** version = (char const**)dlsym(h, handle::versionning);

  // Could not find version symbol.
  if (!version) {
    log_v2::core()->error(
        "modules: could not find version in '{}' (not a Centreon Broker module "
        "?): {}",
        name, dlerror());
    return false;
  }
  if (!*version) {
    log_v2::core()->error(
        "modules: version symbol of module '{}' is empty (not a Centreon "
        "Broker module ?)",
        name);
    return false;
  }

  // Check version.
  if (::strncmp(CENTREON_BROKER_VERSION, *version,
                strlen(CENTREON_BROKER_VERSION) + 1) != 0) {
    log_v2::core()->error(
        "modules: version mismatch in '{}': exepected '{}', found '{}'", name,
        CENTREON_BROKER_VERSION, *version);
    return false;
  }
  return true;
}

/**
 *  Load a plugin.
 *
 *  @param[in] filename File name.
 *  @param[in] arg      Module argument.
 */
void modules::load_file(const std::string& filename, const void* arg) {
  auto found = _handles.find(filename);
  if (found == _handles.end()) {
    log_v2::core()->info(
        "modules: attempt to load '{}' which is already loaded", filename);
    void* h = dlopen(filename.c_str(), RTLD_LAZY);
    if (_check_module(filename, h)) {
      const char*** parents = (const char***)dlsym(h, handle::parents_list);
      if (parents)
        for (const char* p = **parents; p; ++p) {
          auto found = _handles.find(p);
          if (found == _handles.end())
            load_file(p, arg);
        }
    } else {
      dlclose(h);
      return;
    }
    _handles.emplace(filename, std::make_shared<handle>(filename, h, arg));
  } else {
    log_v2::core()->info(
        "modules: attempt to load '{}' which is already loaded", filename);
    found->second->update(arg);
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
      log_v2::config()->error(e.what());
    }
  }

  // Ending log message.
  log_v2::core()->debug("modules: finished loading directory '{}'", dirname);
}

/**
 * @brief Return the number of modules loaded.
 *
 * @return An size_t.
 */
size_t modules::size() const noexcept {
  std::lock_guard<std::mutex> lck(_m_modules);
  return _handles.size();
}
