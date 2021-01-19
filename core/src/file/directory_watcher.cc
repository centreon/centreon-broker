/*
** Copyright 2015 Centreon
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

#include "com/centreon/broker/file/directory_watcher.hh"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>

#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**
 * Default constructor.
 */
directory_watcher::directory_watcher() : _timeout(0) {
  if ((_inotify_instance_id = ::inotify_init()) == -1) {
    int err = errno;
    throw msg_fmt("directory_watcher: couldn't create inotify instance: '{}'",
                  ::strerror(err));
  }
}

/**
 *  Destructor.
 */
directory_watcher::~directory_watcher() {
  ::close(_inotify_instance_id);
}

/**
 *  Add a directory to the list of watched directories.
 *
 *  @param[in] directory  The directory to add.
 */
void directory_watcher::add_directory(std::string const& directory) {
  int id =
      ::inotify_add_watch(_inotify_instance_id, directory.c_str(),
                          IN_CREATE | IN_MODIFY | IN_DELETE | IN_DELETE_SELF);
  if (id == -1) {
    int err = errno;
    throw msg_fmt("directory_watcher: couldn't add directory: '{}'",
                  ::strerror(err));
  }

  char* real_path = ::realpath(directory.c_str(), nullptr);
  _path_to_id[real_path] = id;
  _id_to_path[id] = real_path;
  ::free(real_path);
}

/**
 *  @brief Remove a directory to the list of watched directories.
 *
 *  Do nothing if the directory was not watched.
 *
 *  @param[in] directory  The directory to remove.
 */
void directory_watcher::remove_directory(std::string const& directory) {
  char* real_path = ::realpath(directory.c_str(), nullptr);
  std::map<std::string, int>::iterator it(_path_to_id.find(real_path));
  ::free(real_path);
  if (it == _path_to_id.end())
    return;

  if (::inotify_rm_watch(_inotify_instance_id, it->second) == -1) {
    int err = errno;
    throw msg_fmt("directory_watcher: couldn't remove directory: '{}'",
                  ::strerror(err));
  }

  _id_to_path.erase(it->second);
  _path_to_id.erase(it);
}

/**
 *  Is this directory watched?
 *
 *  @param[in] directory  The directory.
 *
 *  @return  True if this directory is watched.
 */
bool directory_watcher::is_watched(std::string const& directory) {
  return (_path_to_id.find(directory) != _path_to_id.end());
}

/**
 *  @brief Get the events of the watched directories.
 *
 *  This will blocks until new events are available.
 */
std::vector<directory_event> directory_watcher::get_events() {
  std::vector<directory_event> ret;

  // Wait for at least one event.
  fd_set set;
  struct timeval tv;
  FD_ZERO(&set);
  FD_SET(_inotify_instance_id, &set);
  tv.tv_sec = _timeout / 1000;
  tv.tv_usec = (_timeout % 1000) * 1000;
  ::select(_inotify_instance_id + 1, &set, nullptr, nullptr,
           _timeout != 0 ? &tv : nullptr);

  if (!FD_ISSET(_inotify_instance_id, &set))
    return (ret);

  // Get the events
  int buf_size;
  if (ioctl(_inotify_instance_id, FIONREAD, &buf_size) == -1) {
    int err = errno;
    throw msg_fmt("directory_watcher: couldn't read events: '{}'",
                  ::strerror(err));
  }
  logging::debug(logging::medium)
      << "file: directory watcher getting events of size " << buf_size;
  char* buf = new char[buf_size];
  int len = ::read(_inotify_instance_id, buf, buf_size);
  if (len == -1) {
    int err = errno;
    delete[] buf;
    throw msg_fmt("directory_watcher: couldn't read events: '{}'",
                  ::strerror(err));
  }

  // Iterate over all the events.
  struct inotify_event const* event;
  for (char* ev = buf; ev < buf + len;
       ev += sizeof(struct inotify_event) + event->len) {
    event = (const struct inotify_event*)buf;
    directory_event::type event_type;
    if (event->mask & IN_CREATE)
      event_type = directory_event::created;
    else if (event->mask & IN_MODIFY)
      event_type = directory_event::modified;
    else if (event->mask & IN_DELETE)
      event_type = directory_event::deleted;
    else if (event->mask & IN_DELETE_SELF)
      event_type = directory_event::directory_deleted;
    else
      continue;

    std::map<int, std::string>::const_iterator found_path(
        _id_to_path.find(event->wd));
    if (found_path == _id_to_path.end())
      break;

    std::string name = found_path->second + "/" + event->name;

    // Check if it's a file or a directory.
    directory_event::file_type ft = directory_event::other;
    if (event_type != directory_event::deleted &&
        event_type != directory_event::directory_deleted) {
      struct stat st;
      if (::lstat(name.c_str(), &st) == -1) {
        const char* error = ::strerror(errno);
        throw msg_fmt("directory_watcher: couldn't check the file type: '{}'",
                      error);
      }
      ft = directory_event::other;
      if (S_ISDIR(st.st_mode))
        ft = directory_event::directory;
      else if (S_ISREG(st.st_mode))
        ft = directory_event::file;
    }

    ret.push_back(directory_event(name, event_type, ft));
    logging::debug(logging::medium)
        << "file: directory watcher getting an event for path '" << name
        << "' and type " << event_type;
  }

  return (ret);
}

/**
 *  Set the timeout to check event.
 *
 *  @param[in] msecs  The timeout, in milliseconds. 0 for none.
 */
void directory_watcher::set_timeout(uint32_t msecs) {
  _timeout = msecs;
}
