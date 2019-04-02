/*
** Copyright 2015,2017 Centreon
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

#include <QMutexLocker>
#include <QFileInfo>
#include <QFile>
#include <QDirIterator>
#include <cstdio>
#include <set>
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/misc/tokenizer.hh"
#include "com/centreon/broker/dumper/directory_dump.hh"
#include "com/centreon/broker/dumper/directory_dump_committed.hh"
#include "com/centreon/broker/dumper/directory_dumper.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/remove.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/io/events.hh"
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
 *  @param[in] name    The dumper endpoint name.
 *  @param[in] path    Dumper path.
 *  @param[in] tagname Dumper tagname.
 *  @param[in] cache   The persistent cache.
 */
directory_dumper::directory_dumper(
          std::string const& name,
          std::string const& path,
          std::string const& tagname,
          std::shared_ptr<persistent_cache> cache)
  : _name(name.c_str()),
    _path(path),
    _tagname(tagname),
    _cache(cache) {
  try {
    _load_cache();
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
             << "directory_dumper: couldn't load cache for '"
             << _name << "': " << e.what());
  }
}


/**
 *  Destructor.
 */
directory_dumper::~directory_dumper() {
  try {
    _save_cache();
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "directory_dumper: couldn't save cache for '"
      << _name << "': " << e.what();
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
  (void)d;
  (void)deadline;
  throw (exceptions::shutdown() << "cannot read from directory dumper");
  return (true);
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
int directory_dumper::write(misc::shared_ptr<io::data> const& d) {
  if (!validate(d, "directory dumper"))
    return  (1);

  if (d->type() == extcmd::command_request::static_type()) {
    extcmd::command_request const&
      req(d.ref_as<extcmd::command_request const>());
    if (req.is_addressed_to(_name)) {
      _command_to_poller_id[req.uuid.toStdString()] = req.source_id;
      try {
        // Execute command that was especially addressed to us.
        if (req.cmd == "DUMP_DIR")
          _dump_dir(_path, req.uuid);
        else
          throw (exceptions::msg() << "unknown command:"
                 << " the only valid command is DUMP_DIR");
      }
      catch (std::exception const& e) {
        // Log error.
        logging::error(logging::medium)
          << "directory_dumper: couldn't parse '"
          << req.cmd << "': " << e.what();

        // Send error result.
        misc::shared_ptr<extcmd::command_result>
          res(new extcmd::command_result);
        res->uuid = req.uuid;
        res->msg = QString("\"") + e.what() + "\"";
        res->code = -1;
        res->destination_id = req.source_id;
        multiplexing::publisher().write(res);
      }
    }
  }
  else if (d->type() == directory_dump_committed::static_type()) {
    directory_dump_committed const& ddc = d.ref_as<directory_dump_committed>();
    if (_command_to_poller_id.find(ddc.req_id.toStdString()) != _command_to_poller_id.end()) {
      // Send successful result.
      misc::shared_ptr<extcmd::command_result>
        res(new extcmd::command_result);
      res->uuid = ddc.req_id;
      res->msg = "\"Command successfully executed.\"";
      res->code = 0;
      res->destination_id = _command_to_poller_id[ddc.req_id.toStdString()];
      multiplexing::publisher().write(res);
    }
  }

  return (1);
}

/**
 *  Dump the directory of this directory dumper.
 *
 *  @param[in] path               The path of the directory to dump.
 *  @param[in] req_id             The id of the request.
 *  @param[in] command_poller_id  The id of the poller making the request.
 */
void directory_dumper::_dump_dir(
                         std::string const& path,
                         QString const& req_id) {
  logging::debug(logging::medium)
    << "directory_dumper: dumping dir '" << path << "'";

  multiplexing::publisher pblsh;

  QDirIterator dir(
    QString::fromStdString(path),
    QDir::Files | QDir::NoDotAndDotDot,
    QDirIterator::Subdirectories);

  QDir root_dir(QString::fromStdString(path));

  // Start the dump.
  {
    misc::shared_ptr<directory_dump> dmp(new directory_dump);
    dmp->req_id = req_id;
    dmp->tag = QString::fromStdString(_tagname);
    dmp->started = true;
    pblsh.write(dmp);
  }

  // Set of found files.
  std::set<std::string> found;

  while (dir.hasNext()) {
    QString path = dir.next();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
      logging::error(logging::medium)
        << "directory_dumper: can't read file '" << path << "'";
    QByteArray content = file.readAll();
    misc::shared_ptr<dump> dmp(new dump);
    dmp->filename = root_dir.relativeFilePath(path);
    dmp->content = QString(content);
    dmp->tag = QString::fromStdString(_tagname);
    dmp->req_id = req_id;
    pblsh.write(dmp);
    found.insert(dmp->filename.toStdString());
  }

  // Remove all files not found.
  for (std::map<std::string, timestamp_cache>::const_iterator
         it = _files_cache.begin(),
         end = _files_cache.end();
       it != end;
       ++it)
    if (found.find(it->first) == found.end()) {
      misc::shared_ptr<remove> rm(new remove);
      rm->tag = QString::fromStdString(_tagname);
      rm->filename = QString::fromStdString(it->first);
      rm->req_id = req_id;
      pblsh.write(rm);
    }

  // Update the list of files found.
  _files_cache.clear();
  for (std::set<std::string>::const_iterator
         it = found.begin(),
         end = found.end();
       it != end;
       ++it) {
    timestamp_cache tc;
    tc.filename = QString::fromStdString(*it);
    _files_cache[*it] = tc;
  }

  // End the dump.
  {
    misc::shared_ptr<directory_dump> dmp(new directory_dump);
    dmp->req_id = req_id;
    dmp->tag = QString::fromStdString(_tagname);
    dmp->started = false;
    pblsh.write(dmp);
  }
}

/**
 *  Load the cache.
 */
void directory_dumper::_load_cache() {
  // No cache, nothing to do.
  if (_cache.get() == NULL)
    return ;

  misc::shared_ptr<io::data> d;
  while (true) {
    _cache->get(d);
    if (d.isNull())
      return ;
    if (d->type() == timestamp_cache::static_type()) {
     timestamp_cache const& tc  = d.ref_as<timestamp_cache const>();
      _files_cache[tc.filename.toStdString()] = tc;
    }
  }
}

/**
 *  Save the cache.
 */
void directory_dumper::_save_cache() {
  // No cache, nothing to do.
  if (_cache.get() == NULL)
    return ;

  _cache->transaction();
  for (std::map<std::string, timestamp_cache>::const_iterator
         it = _files_cache.begin(),
         end = _files_cache.end();
       it != end;
       ++it)
    _cache->add(misc::make_shared(new timestamp_cache(it->second)));
  _cache->commit();
}
