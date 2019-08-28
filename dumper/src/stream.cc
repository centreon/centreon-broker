/*
** Copyright 2013,2017 Centreon
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

#include <fstream>
#include <sstream>
#include <errno.h>
#include <cstdio>
#include <csignal>
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/directory_dump.hh"
#include "com/centreon/broker/dumper/directory_dump_committed.hh"
#include "com/centreon/broker/dumper/remove.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/stream.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

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
 */
stream::stream(
          std::string const& path,
          std::string const& tagname)
  : _path(path), _tagname(tagname) {
  // Replace macros.
  std::ostringstream oss;
  oss << io::data::broker_id;
  misc::string::replace(_tagname, "$BROKERID$", oss.str());
  oss.str("");
  oss << config::applier::state::instance().poller_id();
  misc::string::replace(_tagname, "$POLLERID$", oss.str());
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw (exceptions::shutdown()
         << "attempt to read from dumper '" << _path << "'");
  return true;
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
int stream::write(std::shared_ptr<io::data> const& d) {
  // Check that data exists.
  if (!validate(d, "dumper"))
    return 1;

  // Check if the event is a dumper event.
  if (d->type() == dump::static_type()) {
    dump const& data = *std::static_pointer_cast<dump const>(d);
    if (data.tag == _tagname) {
      if (data.req_id.empty())
        _process_dump_event(data);
      else
        _add_to_directory_cache(
          data.req_id,
          std::make_shared<dump>(data));
    }
  }
  else if (d->type() == dumper::remove::static_type()) {
    dumper::remove const& data = *std::static_pointer_cast<dumper::remove const>(d);
    if (data.tag == _tagname) {
      if (data.req_id.empty())
        _process_remove_event(*std::static_pointer_cast<dumper::remove const>(d));
      else
        _add_to_directory_cache(
          data.req_id,
          std::make_shared<dumper::remove>(data));
    }
  }
  else if (d->type() == dumper::directory_dump::static_type()
           && std::static_pointer_cast<dumper::directory_dump const>(d)->tag
                 == _tagname) {
    _process_directory_dump_event(*std::static_pointer_cast<dumper::directory_dump const>(d));
  }
  return 1;
}

/**
 *  Process a dump event.
 *
 *  @param[in] data  The dump event.
 */
void stream::_process_dump_event(dump const& data) {
  // Lock mutex.
  std::lock_guard<std::mutex> lock(_mutex);

  logging::debug(logging::medium)
    << "dumper: dumping content of file " << data.filename;

  // Get Broker ID.
  std::ostringstream oss;
  oss << data.source_id;

  // Build path.
  std::string path(_path);
  misc::string::replace(path, "$INSTANCEID$", oss.str());
  misc::string::replace(path, "$BROKERID$", oss.str());
  misc::string::replace(
                  path,
                  "$FILENAME$",
                  data.filename);

  // Get sub directory, if any. Create it if needed.
  size_t pos{path.find_last_of('/')};
  std::string dir;
  if (pos == std::string::npos)
    dir = "/";
  else
    dir = path.substr(0, pos);

  if (!misc::filesystem::dir_exists(dir)) {
    if (!misc::filesystem::mkpath(dir))
      throw exceptions::msg()
        << "dumper: can't create the directory: " << dir;
  }

  // Open file.
  std::ofstream file(path);
  if (!file.is_open())
    throw exceptions::msg()
           << "dumper: error can not open file '"
           << path << "'";

  // Write data.
  file << data.content;
}

/**
 *  Process a remove event.
 *
 *  @param[in] data  The remove event.
 */
void stream::_process_remove_event(remove const& data) {
  // Lock mutex.
  std::lock_guard<std::mutex> lock(_mutex);

  logging::debug(logging::medium)
    << "dumper: removing file " << data.filename;

  // Get Broker ID.
  std::ostringstream oss;
  oss << data.source_id;

  // Build path.
  std::string path(_path);
  misc::string::replace(path, "$INSTANCEID$", oss.str());
  misc::string::replace(path, "$BROKERID$", oss.str());
  misc::string::replace(
                  path,
                  "$FILENAME$",
                  data.filename);

  // Remove file.
  if (::remove(path.c_str()) == -1) {
    const char* msg = ::strerror(errno);
    logging::error(logging::medium)
      << "dumper: can't erase file '" << path << "': " << msg;
  }
}

/**
 *  Process a directory dump event.
 *
 *  @param[in] dmp  The directory dump event.
 */
void stream::_process_directory_dump_event(directory_dump const& dd) {
  // Lock mutex.
  std::lock_guard<std::mutex> lock(_mutex);

  if (dd.started) {
    logging::debug(logging::medium)
      << "dumper: starting directory dump for request " << dd.req_id;
    // Create empty directory cache.
    _cached_directory_dump[dd.req_id];
  }
  else {
    logging::debug(logging::medium)
      << "dumper: committing directory dump for request " << dd.req_id;
    bool success = true;
    std::string error_message;

    directory_dump_cache::iterator found;
    if (found == _cached_directory_dump.end())
      return ;
    std::vector<std::shared_ptr<io::data> > const& events
      = found->second;
    try {
      for (std::vector<std::shared_ptr<io::data> >::const_iterator
             it = events.begin(),
             end = events.end();
           it != end;
           ++it) {
        if ((*it)->type() == dump::static_type())
          _process_dump_event(*std::static_pointer_cast<dump const>(*it));
        else if ((*it)->type() == dumper::remove::static_type())
          _process_remove_event(*std::static_pointer_cast<dumper::remove const>(*it));
      }
    } catch (std::exception const& e) {
      success = false;
      error_message = e.what();
    }

    // Remove directory cache.
    _cached_directory_dump.erase(dd.req_id);

    // Send acknowledgement event.
    {
      std::shared_ptr<directory_dump_committed> ddc(
        new directory_dump_committed);
      ddc->success = success;
      ddc->req_id = dd.req_id;
      ddc->error_message = error_message;
      multiplexing::publisher pblsh;
      pblsh.write(ddc);
    }

      // Reload
    logging::debug(logging::medium)
      << "dumper: reloading";
    ::raise(SIGHUP);
  }
}

/**
 *  Add to a directory cache if it exists.
 *
 *  @param[in] req_id             The id of the request.
 *  @param[in] event              The event to add.
 */
void stream::_add_to_directory_cache(
               std::string const& req_id,
               std::shared_ptr<io::data> event) {
  directory_dump_cache::iterator found
    = _cached_directory_dump.find(req_id);
  if (found == _cached_directory_dump.end())
    return ;
  found->second.push_back(event);
}
