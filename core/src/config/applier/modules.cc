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
    if (!load_file(fmt::format("{}/{}", module_dir, m), arg))
      log_v2::config()->error("module applier: impossible to load module '{}'",
                              m);
  }
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
  if (!h)
    return false;

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

  // Find version symbol.
  log_v2::core()->debug(
      "modules: checking module version (symbol '{}') in '{}'",
      handle::versionning, name);

  const char** version = (const char**)dlsym(h, handle::versionning);

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
 *
 *  @return True on success.
 */
bool modules::load_file(const std::string& filename, const void* arg) {
  auto found = _handles.find(filename);
  if (found == _handles.end()) {
    log_v2::core()->info("modules: attempt to load module '{}'", filename);
    void* h = dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (_check_module(filename, h)) {
      void* parents = dlsym(h, handle::parents_list);
      if (parents) {
        size_t pos = filename.find_last_of('/');
        fmt::string_view path;
        if (pos != std::string::npos)
          path = fmt::string_view(filename.data(), pos);
        else
          path = fmt::string_view(".", 1);
        union {
          const char* const* (*code)();
          void* data;
        } sym;
        sym.data = parents;
        const char* const* pts = (*(sym.code))();
        for (auto p = pts; *p; ++p) {
          auto found = _handles.find(*p);
          if (found == _handles.end())
            if (!load_file(fmt::format("{}/{}", path, *p), arg))
              log_v2::config()->error(
                  "modules: impossible to load parent module '{}'", *p);
        }
      }
    } else {
      if (h)
        dlclose(h);
      return false;
    }
    _handles.emplace(filename, std::make_shared<handle>(filename, h, arg));
  } else {
    log_v2::core()->info(
        "modules: attempt to load '{}' which is already loaded", filename);
    found->second->update(arg);
  }
  return true;
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
