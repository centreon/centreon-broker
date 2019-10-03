/*
** Copyright 2013-2017 Centreon
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

#include <fcntl.h>
#include <rrd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sstream>
#ifdef __linux__
#include <sys/sendfile.h>
#endif  // Linux
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/creator.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**
 *  Constructor.
 *
 *  @param[in] tmpl_path  The template path.
 *  @param[in] cache_size The maximum number of cache element.
 */
creator::creator(std::string const& tmpl_path, uint32_t cache_size)
    : _cache_size(cache_size), _tmpl_path(tmpl_path) {
  logging::debug(logging::medium)
      << "RRD: file creator will maintain at most " << _cache_size
      << " templates in '" << _tmpl_path << "'";
}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
creator::creator(creator const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
creator::~creator() {
  clear();
}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
creator& creator::operator=(creator const& right) {
  if (this != &right) {
    _cache_size = right._cache_size;
    _fds = right._fds;
    _tmpl_path = right._tmpl_path;
  }
  return (*this);
}

/**
 *  Clear cache and remove template file.
 */
void creator::clear() {
  for (std::map<tmpl_info, fd_info>::const_iterator it(_fds.begin()),
       end(_fds.end());
       it != end; ++it) {
    tmpl_info info(it->first);
    ::close(it->second.fd);
    std::ostringstream oss;
    oss << _tmpl_path << "/tmpl_" << info.length << "_" << info.step << "_"
        << info.value_type << ".rrd";
    ::remove(oss.str().c_str());
  }
  _fds.clear();
  return;
}

/**
 *  Create a RRD file if it does not exists.
 *
 *  @param[in] filename   Path to the RRD file.
 *  @param[in] length     Duration in seconds that the RRD file should
 *                        retain.
 *  @param[in] from       Timestamp of the first record.
 *  @param[in] step       Specifies the base interval in seconds with
 *                        which data will be fed into the RRD.
 *  @param[in] value_type Type of the metric.
 */
void creator::create(std::string const& filename,
                     uint32_t length,
                     time_t from,
                     uint32_t step,
                     short value_type) {
  // Fill template informations.
  if (!step)
    step = 5 * 60;  // Default to every 5 minutes.
  if (!length)
    length = 31 * 24 * 60 * 60;  // Default to one month long.
  tmpl_info info;
  info.length = length;
  info.step = step;
  info.value_type = value_type;

  // Find fd informations.
  std::map<tmpl_info, fd_info>::const_iterator it(_fds.find(info));
  // Is in the cache, just duplicate file.
  if (it != _fds.end())
    _duplicate(filename, it->second);
  // Not is the cache, but we have enough space in the cache.
  // Create new entry.
  else if (_fds.size() < _cache_size) {
    std::ostringstream oss;
    oss << _tmpl_path << "/tmpl_" << length << "_" << step << "_" << value_type
        << ".rrd";
    std::string tmpl_filename(oss.str());

    // Create new template.
    _open(tmpl_filename, length, from, step, value_type);

    // Get template file size.
    struct stat s;
    if (stat(tmpl_filename.c_str(), &s) < 0) {
      char const* msg(strerror(errno));
      throw(exceptions::open() << "RRD: could not create template file '"
                               << tmpl_filename << "': " << msg);
    }

    // Get template file fd.
    int in_fd(open(tmpl_filename.c_str(), O_RDONLY));
    if (in_fd < 0) {
      char const* msg(strerror(errno));
      throw(exceptions::open() << "RRD: could not open template file '"
                               << tmpl_filename << "': " << msg);
    }

    // Store fd informations into the cache.
    fd_info fdinfo;
    fdinfo.fd = in_fd;
    fdinfo.size = s.st_size;
    _fds[info] = fdinfo;

    _duplicate(filename, fdinfo);
  }
  // No more space in the cache, juste create rrd file.
  else
    _open(filename, length, from, step, value_type);
}

/**
 *  Duplicate file.
 *
 *  @param[in] filename The file name to save data.
 *  @param[in] in_fd    The fd informations to duplicate file.
 */
void creator::_duplicate(std::string const& filename, fd_info const& in_fd) {
  // Remove previous file.
  remove(filename.c_str());

  int out_fd(open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH));
  if (out_fd < 0) {
    char const* msg(strerror(errno));
    throw(exceptions::open()
          << "RRD: could not create file '" << filename << "': " << msg);
  }

#ifdef __linux__
  // First call(s) to sendfile detect if the kernel support the syscall
  // on any FD (Linux < 2.6.33 only supports writing to socket FD).
  off_t offset(0);
  ssize_t ret;
  do {
    ret = ::sendfile(out_fd, in_fd.fd, &offset, in_fd.size);
  } while ((ret < 0) && (errno == EAGAIN));
  bool fallback((ret < 0) && ((errno == EINVAL) || (errno == ENOSYS)));
  if (!fallback) {
    if (ret < 0) {
      char const* msg(strerror(errno));
      throw(exceptions::open()
            << "RRD: could not create file '" << filename << "': " << msg);
    }
    // Good to go with the sendfile syscall.
    _sendfile(out_fd, in_fd.fd, ret, in_fd.size, filename);
  } else
#endif  // Linux
    // We must fallback to the read/write combo.
    _read_write(out_fd, in_fd.fd, in_fd.size, filename);

  ::close(out_fd);
}

/**
 *  Open a RRD file and create it if it does not exists.
 *
 *  @param[in] filename   Path to the RRD file.
 *  @param[in] length     Duration in seconds that the RRD file should
 *                        retain.
 *  @param[in] from       Timestamp of the first record.
 *  @param[in] step       Time interval between each record.
 *  @param[in] value_type Type of the metric.
 */
void creator::_open(std::string const& filename,
                    uint32_t length,
                    time_t from,
                    uint32_t step,
                    short value_type) {
  /* Find step of RRD file if already existing. */
  /* XXX : why is it here ?
  rrd_info_t* rrdinfo(rrd_info_r(_filename));
  time_t interval_offset(0);
  for (rrd_info_t* tmp = rrdinfo; tmp; tmp = tmp->next)
    if (!strcmp(rrdinfo->key, "step"))
      if (interval < static_cast<time_t>(rrdinfo->value.u_cnt))
        interval_offset = rrdinfo->value.u_cnt / interval - 1;
  rrd_info_free(rrdinfo);
  */

  //
  // Set rrd_create_r() parameters array.
  //

  char const* argv[5];
  int argc(0);

  // DS.
  std::string ds;
  {
    std::ostringstream oss;
    oss << "DS:value:";
    switch (value_type) {
      case storage::perfdata::absolute:
        oss << "ABSOLUTE";
        break;
      case storage::perfdata::counter:
        oss << "COUNTER";
        break;
      case storage::perfdata::derive:
        oss << "DERIVE";
        break;
      default:
        oss << "GAUGE";
    };
    oss << ":" << step * 10 << ":U:U";
    ds = oss.str();
    argv[argc++] = ds.c_str();
  }

  // Base RRA.
  std::string rra1;
  {
    std::ostringstream oss;
    oss << "RRA:AVERAGE:0.5:" << step << ":" << length / step + 1;
    rra1 = oss.str();
    argv[argc++] = rra1.c_str();
  }

  // Aggregate RRA.
  std::string rra2;
  if (step < 3600) {
    std::ostringstream oss;
    oss << "RRA:AVERAGE:0.5:" << 3600 << ":" << length / 3600 + 1;
    rra2 = oss.str();
    argv[argc++] = rra2.c_str();
  }

  // Debug message.
  argv[argc] = nullptr;
  logging::debug(logging::high)
      << "RRD: opening file '" << filename << "' (" << argv[0] << ", "
      << argv[1] << ", " << (argv[2] ? argv[2] : "(null)") << ", step 1, from "
      << from << ")";

  // Create RRD file.
  rrd_clear_error();
  if (rrd_create_r(filename.c_str(), 1, from, argc, argv))
    throw(exceptions::open() << "RRD: could not create file '" << filename
                             << "': " << rrd_get_error());

  return;
}

/**
 *  Transfer file between two FDs using read and write.
 *
 *  @param[in] out_fd   Output FD.
 *  @param[in] in_fd    Input FD.
 *  @param[in] size     Size to transfer.
 *  @param[in] filename Path to the file being created.
 */
void creator::_read_write(int out_fd,
                          int in_fd,
                          ssize_t size,
                          std::string const& filename) {
  // Reset position of in_fd.
  if (lseek(in_fd, 0, SEEK_SET) == (off_t)-1) {
    char const* msg(strerror(errno));
    throw(exceptions::open()
          << "RRD: could not create file '" << filename << "': " << msg);
  }

  char buffer[4096];
  ssize_t transfered(0);
  while (transfered < size) {
    // Read from in_fd.
    ssize_t rb(::read(in_fd, buffer, sizeof(buffer)));
    if (rb <= 0) {
      if (errno != EAGAIN) {
        char const* msg(strerror(errno));
        throw(exceptions::open()
              << "RRD: could not create file '" << filename << "': " << msg);
      }
      continue;
    }

    // Write to out_fd.
    ssize_t wb(0);
    while (wb < rb) {
      ssize_t ret(::write(out_fd, buffer + wb, rb - wb));
      if (ret <= 0) {
        if (errno != EAGAIN) {
          char const* msg(strerror(errno));
          throw(exceptions::open()
                << "RRD: could not create file '" << filename << "': " << msg);
        }
      } else
        wb += ret;
    }

    // Update total transfered bytes.
    transfered += wb;
  }
  return;
}

#ifdef __linux__
/**
 *  Transfer file between two FDs using sendfile.
 *
 *  @param[in] out_fd             Output FD.
 *  @param[in] in_fd              Input FD.
 *  @param[in] already_transfered Number of bytes already transfered.
 *  @param[in] size               Total size to transfer.
 *  @param[in] filename           Path to the file being created.
 */
void creator::_sendfile(int out_fd,
                        int in_fd,
                        off_t already_transfered,
                        ssize_t size,
                        std::string const& filename) {
  ssize_t total(already_transfered);
  while (total < size) {
    already_transfered = total;
    ssize_t ret = ::sendfile(out_fd, in_fd, &already_transfered,
                             size - already_transfered);
    if ((ret <= 0) && (errno != EAGAIN)) {
      char const* msg(strerror(errno));
      throw(exceptions::open()
            << "RRD: could not create file '" << filename << "': " << msg);
    } else if (ret > 0)
      total += ret;
  }
  return;
}
#endif  // Linux
