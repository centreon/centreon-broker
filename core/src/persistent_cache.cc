/*
** Copyright 2014-2015 Merethis
**
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

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <QFile>
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] cache_file  Path to the cache file.
 */
persistent_cache::persistent_cache(std::string const& cache_file)
  : _cache_file(cache_file) {
  _open();
}

/**
 *  Destructor.
 */
persistent_cache::~persistent_cache() {}

/**
 *  @brief Add an event to the persistent cache.
 *
 *  The event will be serialized and stored in the cache file.
 *
 *  @param[in] d  Object to store.
 */
void persistent_cache::add(misc::shared_ptr<io::data> const& d) {
  if (_write_file.isNull())
    throw (exceptions::msg() << "core: cache file '"
           << _cache_file << "' is not open for writing");
  _write_file->write(d);
  return ;
}

/**
 *  @brief Apply changes to the persistent cache.
 *
 *  After a temporary persistent cache has been written through multiple
 *  add() calls, use this function to delete the previous cache and make
 *  the new file the persistent cache.
 */
void persistent_cache::commit() {
  // Perform changes only if a transaction was started.
  if (!_write_file.isNull()) {
    _write_file.clear();
    _read_file.clear();
    if (::rename(_cache_file.c_str(), _old_file().c_str())) {
      char const* msg(strerror(errno));
      throw (exceptions::msg() << "core: cache file '"
             << _cache_file << "' could not be renamed to '"
             << _old_file() << "': " << msg);
    }
    else if (::rename(_new_file().c_str(), _cache_file.c_str())) {
      // .old file will be renamed by the _open() method.
      char const* msg(strerror(errno));
      throw (exceptions::msg() << "core: cache file '"
             << _new_file() << "' could not be renamed to '"
             << _cache_file << "': " << msg);
    }
    // No error checking, this is a secondary issue.
    ::remove(_old_file().c_str());
  }
  return ;
}

/**
 *  Get the next event from the persistent cache.
 *
 *  @param[out] d  Pointer to the next event of the persistent cache. A
 *                 NULL pointer is provided on EOF.
 */
void persistent_cache::get(misc::shared_ptr<io::data>& d) {
  if (_read_file.isNull())
    _open();
  try {
    _read_file->read(d);
  }
  catch (io::exceptions::shutdown const& e) {
    (void)e;
    d.clear();
  }
  return ;
}

/**
 *  Rollback a transaction.
 */
void persistent_cache::rollback() {
  _write_file.clear();
  ::remove(_new_file().c_str());
  return ;
}

/**
 *  @brief Start a transaction (a new persistent cache file).
 *
 *  The old cache won't be erased until commit() is called.
 */
void persistent_cache::transaction() {
  if (!_write_file.isNull())
    throw (exceptions::msg() << "core: cache file '"
           << _cache_file << "' is already open for writing");
  misc::shared_ptr<file::stream> fs(new file::stream(_new_file()));
  misc::shared_ptr<bbdo::stream> bs(new bbdo::stream(false, true));
  bs->read_from(fs);
  bs->write_to(fs);
  _write_file = bs.staticCast<io::stream>();
  return ;
}

/**
 *  Get the name of the cache file.
 *
 *  @return  The name of the cache file.
 */
std::string const& persistent_cache::get_cache_file() const {
  return (_cache_file);
}

/**
 *  Get the new file name.
 *
 *  @return Cache file name appended with ".new".
 */
std::string persistent_cache::_new_file() const {
  std::string new_file(_cache_file);
  new_file.append(".new");
  return (new_file);
}

/**
 *  Get the old file name.
 *
 *  @return Cache file name appended with ".old".
 */
std::string persistent_cache::_old_file() const {
  std::string old_file(_cache_file);
  old_file.append(".old");
  return (old_file);
}

/**
 *  Open persistent cache file.
 */
void persistent_cache::_open() {
  // Open either cache file or old cache file.
  if (!QFile::exists(_cache_file.c_str())) {
    if (QFile::exists(_old_file().c_str()))
      ::rename(_old_file().c_str(), _cache_file.c_str());
  }

  // Create file stream.
  misc::shared_ptr<file::stream> fs(new file::stream(_cache_file));
  fs->set_auto_delete(false);
  fs->reset();

  // Create BBDO layer.
  misc::shared_ptr<bbdo::stream> bs(new bbdo::stream(true, false));
  bs->read_from(fs);
  bs->write_to(fs);

  // We will access only the BBDO layer.
  _read_file = bs.staticCast<io::stream>();

  return ;
}
