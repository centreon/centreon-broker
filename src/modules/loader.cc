/*
** Copyright 2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <QDir>
#include "modules/loader.hh"

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
loader::~loader() {}

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
 *  Load a directory containing plugins.
 *
 *  @param[in] dirname Directory name.
 */
void loader::load_dir(QString const& dirname) {
  // Set directory browsing parameters.
  QDir dir(dirname);
  QStringList l;
  l.push_back("*.so");
  dir.setNameFilters(l);

  // Iterate through all modules in directory.
  l = dir.entryList();
  for (QStringList::iterator it = l.begin(), end = l.end();
       it != end;
       ++it) {
    QString file(dirname);
    file.append("/");
    file.append(*it);
    load_file(file);
  }

  return ;
}

/**
 *  Load a plugin.
 *
 *  @param[in] filename File name.
 */
void loader::load_file(QString const& filename) {
  QSharedPointer<handle> handl(new handle);
  handl->open(filename);
  _handles.push_back(handl);
  return ;
}
