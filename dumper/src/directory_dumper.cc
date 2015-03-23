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
#include <fstream>
#include <sstream>
#include "com/centreon/broker/dumper/directory_dumper.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/dump.hh"
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
  : _path(path),
    _process_in(true),
    _process_out(true),
    _tagname(tagname),
    _cache(cache) {
  _get_last_timestamps_from_cache();
  _set_watch_over_directory();
}

/**
 *  Destructor.
 */
directory_dumper::~directory_dumper() {
  try {
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
  QMutexLocker lock(&_mutex);
  _process_in = in;
  _process_out = in || !out;
  return ;
}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d Bunch of data.
 */
void directory_dumper::read(misc::shared_ptr<io::data>& d) {
  d.clear();

  // Get an event already in the event list.
  if (!_event_list.empty()) {
    _last_modified_timestamps[_event_list.front().second->filename.toStdString()]
      = _event_list.front().first;
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
    _event_list.push_back(_dump_a_file(it->get_path()));
  }

  // Get an event in the new event list.
  if (!_event_list.empty()) {
    _last_modified_timestamps[_event_list.front().second->filename.toStdString()]
      = _event_list.front().first;
    d = _event_list.front().second;
    _event_list.pop_front();
  }
  return ;
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
  if (directory_info.exists())
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
  QDirIterator dir(QString::fromStdString(_path));
  while (dir.hasNext()) {
    QString filepath = dir.next();
    std::map<std::string, timestamp>::const_iterator found_timestamp(
        _last_modified_timestamps.find(filepath.toStdString()));
    if (found_timestamp == _last_modified_timestamps.end()
          || found_timestamp->second < QFileInfo(filepath).lastModified().toTime_t())
      _event_list.push_back(_dump_a_file(filepath.toStdString()));
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
std::pair<timestamp, misc::shared_ptr<dump> > directory_dumper::_dump_a_file(
                             std::string const& path) {
  QFile file(QString::fromStdString(path));

  if (!file.isReadable())
    throw (exceptions::msg()
           << "dumper: can't read '" << path << "'");
  timestamp ts(::time(NULL));
  QString content = file.readAll();

  misc::shared_ptr<dumper::dump> dump(new dumper::dump);
  dump->filename = QString::fromStdString(path);
  dump->content = content;
  dump->tag = QString::fromStdString(_tagname);
  return (std::make_pair(ts, dump));
}
