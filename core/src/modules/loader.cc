/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/modules/loader.hh"
#include <sys/stat.h>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::modules;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Destructor.
 */
loader::~loader() {
  unload();
}

/**
 *  Get iterator to the first module.
 *
 *  @return Iterator to the first module.
 */
loader::iterator loader::begin() {
  return (_handles.begin());
}

/**
 *  Get last iterator of the module list.
 *
 *  @return Last iterator of the module list.
 */
loader::iterator loader::end() {
  return (_handles.end());
}

/**
 *  Load a directory containing plugins.
 *
 *  @param[in] dirname Directory name.
 *  @param[in] arg     Module argument.
 */
void loader::load_dir(std::string const& dirname, void const* arg) {
  // Debug message.
  logging::debug(logging::medium)
      << "modules: loading directory '" << dirname << "'";

  // Set directory browsing parameters.
  std::list<std::string> list;
  list = misc::filesystem::dir_content_with_filter(dirname, "*.so");
  list.sort();

  for (std::list<std::string>::iterator it(list.begin()), end(list.end());
       it != end; ++it) {
    try {
      load_file(*it, arg);
    } catch (msg_fmt const& e) {
      logging::error(logging::high) << e.what();
    }
  }

  // Ending log message.
  logging::debug(logging::medium)
      << "modules: finished loading directory '" << dirname << "'";
}

/**
 *  Load a plugin.
 *
 *  @param[in] filename File name.
 *  @param[in] arg      Module argument.
 */
void loader::load_file(std::string const& filename, void const* arg) {
  auto it = _handles.find(filename);
  if (it == _handles.end()) {
    std::shared_ptr<handle> handl(new handle);
    handl->open(filename, arg);
    _handles[filename] = handl;
  } else {
    logging::info(logging::low) << "modules: attempt to load '" << filename
                                << "' which is already loaded";
    it->second->update(arg);
  }
}

/**
 *  Unload modules.
 */
void loader::unload() {
  auto rit = _handles.rbegin();
  while (rit != _handles.rend()) {
    _handles.erase(rit->first);
    rit = _handles.rbegin();
  }
}
