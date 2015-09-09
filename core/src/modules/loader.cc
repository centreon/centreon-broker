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

#include <QDir>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/modules/loader.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::modules;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
loader::loader() {}

/**
 *  Copy constructor.
 *
 *  @param[in] l Object to copy.
 */
loader::loader(loader const& l) : _handles(l._handles) {}

/**
 *  Destructor.
 */
loader::~loader() {
  unload();
}

/**
 *  Assignment operator.
 *
 *  @param[in] l Object to copy.
 *
 *  @return This object.
 */
loader& loader::operator=(loader const& l) {
  _handles = l._handles;
  return (*this);
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
void loader::load_dir(QString const& dirname, void const* arg) {
  // Debug message.
  logging::debug(logging::medium)
    << "modules: loading directory '" << dirname << "'";

  // Set directory browsing parameters.
  QDir dir(dirname);
  QStringList l;
#ifdef Q_OS_WIN32
  l.push_back("*.dll");
#else
  l.push_back("*.so");
#endif /* Q_OS_WIN32 */
  dir.setNameFilters(l);

  // Iterate through all modules in directory.
  l = dir.entryList();
  for (QStringList::iterator it = l.begin(), end = l.end();
       it != end;
       ++it) {
    QString file(dirname);
    file.append("/");
    file.append(*it);
    try {
      load_file(file, arg);
    }
    catch (exceptions::msg const& e) {
      logging::error(logging::high) << e.what();
    }
  }

  // Ending log message.
  logging::debug(logging::medium)
    << "modules: finished loading directory '" << dirname << "'";

  return ;
}

/**
 *  Load a plugin.
 *
 *  @param[in] filename File name.
 *  @param[in] arg      Module argument.
 */
void loader::load_file(QString const& filename, void const* arg) {
  QHash<QString, misc::shared_ptr<handle> >::iterator
    it(_handles.find(filename));
  if (it == _handles.end()) {
    misc::shared_ptr<handle> handl(new handle);
    handl->open(filename, arg);
    _handles[filename] = handl;
  }
  else {
    logging::info(logging::low) << "modules: attempt to load '"
      << filename << "' which is already loaded";
    (*it)->update(arg);
  }
  return ;
}

/**
 *  Unload modules.
 */
void loader::unload() {
  QString key;
  while (!_handles.empty()) {
    QHash<QString, misc::shared_ptr<handle> >::iterator
      it(_handles.begin());
    key = it.key();
    QHash<QString, misc::shared_ptr<handle> >::iterator
      end(_handles.end());
    while (++it != end)
      if (it.key() > key)
        key = it.key();
    _handles.remove(key);
  }
  return ;
}
