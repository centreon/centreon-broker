/*
** Copyright 2015 Merethis
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

#include <QMutexLocker>
#include <QFileInfo>
#include <QFile>
#include <QDirIterator>
#include <QDateTime>
#include <set>
#include <fstream>
#include <sstream>
#include "com/centreon/broker/dumper/directory_dumper.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/remove.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] path    Dumper path.
 *  @param[in] tagname Dumper tagname.
 *  @param[in] cache   Persistent cache.
 */
directory_dumper::directory_dumper(
          std::string const& path,
          std::string const& tagname,
          misc::shared_ptr<persistent_cache> cache)
  try :
    _path(path),
    _tagname(tagname),
    _cache(cache) {
  // Set watcher timeout.
  _watcher.set_timeout(3000);

  // Get all the last modified timestamps from the cache.
  _get_last_timestamps_from_cache();

  // Set the watch and the initial events.
  std::set<std::string> files_found = _set_watch_over_directory(path);

  // Remove deleted files.
  _remove_deleted_files(files_found);
}
catch (std::exception const& e) {
  throw (exceptions::msg()
         << "dumper: directory dumper couldn't initialize: " << e.what());
}

/**
 *  Destructor.
 */
directory_dumper::~directory_dumper() {
  try {
    // Save the last modified timestamps to the cache.
    _save_last_timestamps_to_cache();
  } catch (std::exception const& e) {
    logging::error(logging::medium)
    << "dump: directory dumper error while trying to save the cache: "
    << e.what();
  }
}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool directory_dumper::read(
                         misc::shared_ptr<io::data>& d,
                         time_t deadline) {
  d.clear();

  // Get an event already in the event list.
  if (!_event_list.empty()) {
    unsigned int type = _event_list.front().second->type();
    if (type == dump::static_type())
      _last_modified_timestamps[
         _event_list.front().second.ref_as<dump>().filename.toStdString()]
         = _event_list.front().first;
    else if (type == dumper::remove::static_type())
      _last_modified_timestamps.erase(
        _event_list.front().second.ref_as<dumper::remove>().filename.toStdString());
    d = _event_list.front().second;
    _event_list.pop_front();
    return (true);
  }

  // If no events, watch the directory for new events.
  std::vector<file::directory_event> events = _watcher.get_events();
  for (std::vector<file::directory_event>::const_iterator
         it(events.begin()),
         end(events.end());
       it != end;
       ++it) {
    if (it->get_type() == file::directory_event::directory_deleted
          && it->get_path() == _path)
      throw (exceptions::msg()
             << "dumper: directory '" << _path << "' deleted");
    else if (it->get_type() == file::directory_event::deleted) {
      misc::shared_ptr<dumper::remove> d(new dumper::remove);
      d->filename = QString::fromStdString(
                      _get_relative_filename(it->get_path()));
      d->tag = QString::fromStdString(_tagname);
      _event_list.push_back(std::make_pair(timestamp(), d));
    }
    else if (it->get_type() == file::directory_event::created
               && it->get_file_type() == file::directory_event::directory) {
      _set_watch_over_directory(it->get_path());
    }
    else if (it->get_file_type() == file::directory_event::file)
      _event_list.push_back(_dump_a_file(it->get_path()));
  }
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
unsigned int directory_dumper::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (io::exceptions::shutdown(false, true)
         << "cannot write to a dumper directory");
  return (1);
}

/**
 *  Get the list of last modified timestamps from the persistent cache.
 */
void directory_dumper::_get_last_timestamps_from_cache() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  misc::shared_ptr<io::data> d;
  while (true) {
    _cache->get(d);
    if (d.isNull())
      return;
    if (d->type() == timestamp_cache::static_type()) {
      timestamp_cache const& ts = d.ref_as<timestamp_cache const>();
      _last_modified_timestamps[ts.filename.toStdString()] = ts.last_modified;
    }
  }
}

/**
 *  Save the list of last modified timestamps to the persistent cache.
 */
void directory_dumper::_save_last_timestamps_to_cache() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  _cache->transaction();
  for (std::map<std::string, timestamp>::const_iterator
         it(_last_modified_timestamps.begin()),
         end(_last_modified_timestamps.end());
       it != end;
       ++it) {
    misc::shared_ptr<timestamp_cache> d(new timestamp_cache);
    d->filename = QString::fromStdString(it->first);
    d->last_modified = it->second;
    _cache->add(d);
  }
  _cache->commit();
}

/**
 *  Set the watch over the directory.
 *
 *  @param[in] path  The directory path.
 *
 *  @return  All the files which were found and dumper.
 */
std::set<std::string> directory_dumper::_set_watch_over_directory(std::string const& path) {
  // Basic checks.
  QFileInfo directory_info(QString::fromStdString(path));
  if (!directory_info.exists())
    throw (exceptions::msg()
           << "dumper: directory dumper path '" << path << "' doesn't exist");
  if (!directory_info.isDir())
    throw (exceptions::msg()
           << "dumper: directory dumper path '" << path
           << "' is not a directory");
  if (!directory_info.isReadable())
    throw (exceptions::msg()
           << "dumper: directory dumper path '" << path
           << "' can not be accessed");

  // Add the directory to the directory watcher.
  try {
    _watcher.add_directory(path);
  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "dumper: " << e.what());
  }

  // Dump all the files that weren't already dumped at last once
  // using last modified cached timestamp.
  std::set<std::string> found_files;
  {
    QDirIterator dir(
      QDir(QString::fromStdString(path),
      QString(),
      QDir::Name | QDir::IgnoreCase,
      QDir::Files));
    while (dir.hasNext()) {
      QString filepath = dir.next();
      std::map<std::string, timestamp>::const_iterator found_timestamp(
          _last_modified_timestamps.find(filepath.toStdString()));
      if (found_timestamp == _last_modified_timestamps.end()
            || found_timestamp->second < QFileInfo(filepath).lastModified().toTime_t())
        _event_list.push_back(_dump_a_file(filepath.toStdString()));
      found_files.insert(filepath.toStdString());
    }
  }

  // Iterate over directories.
  {
    QDirIterator dir(
      QDir(QString::fromStdString(path),
      QString(),
      QDir::Name | QDir::IgnoreCase,
      QDir::Dirs | QDir::NoDotAndDotDot));
    while (dir.hasNext()) {
      QString filepath = dir.next();
      std::set<std::string> ret = _set_watch_over_directory(
                                    filepath.toStdString());
      found_files.insert(ret.begin(), ret.end());
    }
  }
  return (found_files);
}

/**
 *  Remove all the files which were deleted.
 *
 *  @param[in] found_files  List of files found.
 */
void directory_dumper::_remove_deleted_files(
       std::set<std::string> const& found_files) {
  // Every file that wasn't found has been deleted
  for (std::map<std::string, timestamp>::const_iterator
         it(_last_modified_timestamps.begin()),
         end(_last_modified_timestamps.end());
       it != end;
       ++it)
    if (found_files.find(it->first) == found_files.end()) {
      misc::shared_ptr<dumper::remove> d(new dumper::remove);
      d->filename = QString::fromStdString(it->first);
      d->tag = QString::fromStdString(_tagname);
      _event_list.push_back(std::make_pair(timestamp(), d));
    }
}

/**
 *  Dump a file.
 *
 *  @param[in] path  The path of the file.
 *
 *  @return          A pair of a timestamp containing the time of the dump,
 *                   and an io::data dump containing the file.
 */
std::pair<timestamp, misc::shared_ptr<io::data> > directory_dumper::_dump_a_file(
                             std::string const& path) {
  QFile file(QString::fromStdString(path));

  file.open(QFile::ReadOnly);
  if (!file.isReadable())
    throw (exceptions::msg()
           << "dumper: can't read '" << path << "'");
  timestamp ts(::time(NULL));
  QString content = file.readAll();

  misc::shared_ptr<dumper::dump> dump(new dumper::dump);
  dump->filename = QString::fromStdString(_get_relative_filename(path));
  dump->content = content;
  dump->tag = QString::fromStdString(_tagname);
  return (std::make_pair(ts, dump));
}

/**
 *  Get a filename relative to the path being watched.
 *
 *  @param[in] path  The path being watched.
 *
 *  @return  The filename.
 */
std::string directory_dumper::_get_relative_filename(std::string const& path) {
  std::string ret = path;
  ret.replace(0, _path.size(), "");
  return (ret);
}
