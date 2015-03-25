/*
** Copyright 2013 Merethis
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

extern unsigned int instance_id;

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
  : _path(path),
    _process_in(true),
    _process_out(true),
    _tagname(tagname),
    _cache(cache) {
  // Set watcher timeout.
  _watcher.set_timeout(3000);

  // Get all the last modified timestamps from the cache.
  _get_last_timestamps_from_cache();

  // Set the watch and the initial events.
  _set_watch_over_directory();
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
 *  Set processing flags.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void directory_dumper::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
  return ;
}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d Bunch of data.
 */
void directory_dumper::read(misc::shared_ptr<io::data>& d) {
  d.clear();

  // If closed, do nothing.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "directory dumper stream is shutdown");

  // Get an event already in the event list.
  if (!_event_list.empty()) {
    int type = _event_list.front().second->type();
    if (type == dump::static_type())
      _last_modified_timestamps[
         _event_list.front().second.ref_as<dump>().filename.toStdString()]
         = _event_list.front().first;
    else if (type == remove::static_type())
      _last_modified_timestamps.erase(
        _event_list.front().second.ref_as<remove>().filename.toStdString());
    d = _event_list.front().second;
    _event_list.pop_front();
    return ;
  }

  // If no events, watch the directory for new events.
  std::vector<directory_event> events = _watcher.get_events();
  for (std::vector<directory_event>::const_iterator
         it(events.begin()),
         end(events.end());
       it != end;
       ++it) {
    if (it->get_type() == directory_event::directory_deleted)
      throw (exceptions::msg()
             << "dumper: directory '" << _path << "' deleted");
    else if (it->get_type() == directory_event::deleted) {
      misc::shared_ptr<remove> d(new remove);
      d->filename = QFileInfo(it->get_path().c_str()).baseName();
      d->instance_id = instance_id;
      d->tag = QString::fromStdString(_tagname);
      _event_list.push_back(std::make_pair(timestamp(), d));
    }
    else
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
  throw (exceptions::msg()
         << "dumper: attempt to write from a directory dumper stream");
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
 */
void directory_dumper::_set_watch_over_directory() {
  // Basic checks.
  QFileInfo directory_info(QString::fromStdString(_path));
  if (!directory_info.exists())
    throw (exceptions::msg()
           << "dumper: directory dumper path '" << _path << "' doesn't exist");
  if (!directory_info.isDir())
    throw (exceptions::msg()
           << "dumper: directory dumper path '" << _path
           << "' is not a directory");
  if (!directory_info.isReadable())
    throw (exceptions::msg()
           << "dumper: directory dumper path '" << _path
           << "' can not be accessed");

  // Add the directory to the directory watcher.
  try {
    _watcher.add_directory(_path);
  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "dumper: " << e.what());
  }

  // Dump all the files that weren't already dumped at last once
  // using last modified cached timestamp.
  std::set<std::string> found_files;
  QDirIterator dir(
    QDir(QString::fromStdString(_path),
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

  // Every file that wasn't found has been deleted
  for (std::map<std::string, timestamp>::const_iterator
         it(_last_modified_timestamps.begin()),
         end(_last_modified_timestamps.end());
       it != end;
       ++it)
    if (found_files.find(it->first) == found_files.end()) {
      misc::shared_ptr<remove> d(new remove);

      d->filename = QString::fromStdString(it->first);
      d->tag = QString::fromStdString(_tagname);
      d->instance_id = instance_id;
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
  dump->filename = QFileInfo(path.c_str()).baseName();
  dump->content = content;
  dump->tag = QString::fromStdString(_tagname);
  dump->instance_id = instance_id;
  return (std::make_pair(ts, dump));
}
