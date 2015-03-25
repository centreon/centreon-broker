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

#include <sys/select.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/dumper/directory_watcher.hh"

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 * Default constructor.
 */
directory_watcher::directory_watcher()
  : _timeout(0) {
  if ((_inotify_instance_id = ::inotify_init()) == -1) {
    int err = errno;
    throw (exceptions::msg()
           << "directory_watcher: couldn't create inotify instance: '"
           << ::strerror(err) << "'");
  }

}

/**
 *  Copy constructor.
 *
 *  @param[in] o  The object to copy.
 */
directory_watcher::directory_watcher(directory_watcher const& o)
  : _inotify_instance_id(o._inotify_instance_id),
    _timeout(o._timeout),
    _path_to_id(o._path_to_id),
    _id_to_path(o._id_to_path) {
}

/**
 *  Assignment operator.
 *
 *  @param[in] o  The object to copy.
 *
 *  @return       A reference to this object.
 */
directory_watcher& directory_watcher::operator=(directory_watcher const& o) {
  if (this != &o) {
    _inotify_instance_id = o._inotify_instance_id;
    _timeout = o._timeout;
    _path_to_id = o._path_to_id;
    _id_to_path = o._id_to_path;
  }
  return (*this);
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
  int id = ::inotify_add_watch(
                _inotify_instance_id,
                directory.c_str(),
                IN_CREATE | IN_MODIFY | IN_DELETE | IN_DELETE_SELF);
  if (id == -1) {
    int err = errno;
    throw (exceptions::msg()
           << "directory_watcher: couldn't add directory: '"
           << ::strerror(err) << "'");
  }

  char *real_path = ::realpath(directory.c_str(), NULL);
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
  char *real_path = ::realpath(directory.c_str(), NULL);
  std::map<std::string, int>::iterator it(_path_to_id.find(real_path));
  ::free(real_path);
  if (it == _path_to_id.end())
    return ;

  if (::inotify_rm_watch(_inotify_instance_id, it->second) == -1) {
    int err = errno;
    throw (exceptions::msg()
           << "directory_watcher: couldn't remove directory: '"
           << ::strerror(err) << "'");
  }

  _id_to_path.erase(it->second);
  _path_to_id.erase(it);
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
  ::select(
    _inotify_instance_id + 1,
    &set,
    NULL,
    NULL,
    _timeout != 0 ? &tv : NULL);

  if (!FD_ISSET(_inotify_instance_id, &set))
    return (ret);

  // Get the events
  int buf_size;
  if (ioctl(_inotify_instance_id, FIONREAD, &buf_size) == -1) {
    int err = errno;
    throw (exceptions::msg()
           << "directory_watcher: couldn't read events: '"
           << ::strerror(err) << "'");
  }
  logging::debug(logging::medium)
    << "dumper: directory watcher getting events of size " << buf_size;
  char *buf = new char[buf_size];
  int len = ::read(_inotify_instance_id, buf, buf_size);
  if (len == -1) {
    int err = errno;
    delete [] buf;
    throw (exceptions::msg()
           << "directory_watcher: couldn't read events: '"
           << ::strerror(err) << "'");
  }

  // Iterate over all the events.
  const struct inotify_event *event;
  for (char* ev = buf;
       ev < buf + len;
       ev += sizeof(struct inotify_event) + event->len) {
    event = (const struct inotify_event*) buf;
    directory_event::type event_type;
    if (event->mask & IN_CREATE)
      event_type = directory_event::created;
    else if (event->mask & IN_MODIFY)
      event_type = directory_event::modified;
    else if (event->mask & IN_DELETE)
      event_type = directory_event::deleted;
    else if (event->mask & IN_DELETE_SELF)
      event_type = directory_event::directory_deleted;

    std::map<int, std::string>::const_iterator found_path(
                                                 _id_to_path.find(event->wd));
    if (found_path == _id_to_path.end())
      break ;

    std::string name = found_path->second + "/" + event->name;
    ret.push_back(directory_event(name, event_type));
    logging::debug(logging::medium)
      << "dumper: directory watcher getting an event for path '"
      << name << "' and type " << event_type;
  }

  return (ret);
}

/**
 *  Set the timeout to check event.
 *
 *  @param[in] msecs  The timeout, in milliseconds. 0 for none.
 */
void directory_watcher::set_timeout(unsigned int msecs) {
  _timeout = msecs;
}
