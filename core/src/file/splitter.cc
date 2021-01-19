/*
** Copyright 2020 Centreon
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

#include "com/centreon/broker/file/splitter.hh"

#include <arpa/inet.h>
#include <fmt/format.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <list>
#include <memory>

#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/cfile.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**
 *  Build a new splitter.
 *
 *  @param[in] path           Base path to file.
 *  @param[in] mode           Ignored. Files will always be open
 *                            read/write.
 *  @param[in] max_file_size  Maximum single file size.
 *  @param[in] auto_delete    True to delete file parts as they are
 *                            read.
 */
splitter::splitter(std::string const& path,
                   fs_file::open_mode mode,
                   long max_file_size,
                   bool auto_delete)
    : _auto_delete{auto_delete},
      _base_path{path},
      _max_file_size{max_file_size},
      _rfile{},
      _rmutex{nullptr},
      _rid{0},
      _roffset{0},
      _wfile{},
      _wmutex{nullptr},
      _wid{0},
      _woffset{0} {
  (void)mode;

  // Set max file size.
  static long min_file_size(10000);
  if (!_max_file_size)
    _max_file_size = std::numeric_limits<long>::max();
  else if (_max_file_size < min_file_size)
    _max_file_size = min_file_size;

  // Get IDs of already existing file parts. File parts are suffixed
  // with their order number. A file named /var/lib/foo would have
  // parts named /var/lib/foo, /var/lib/foo1, /var/lib/foo2, ...
  // in this order.
  std::string base_dir;
  std::string base_name;
  {
    size_t last_slash(_base_path.find_last_of('/'));
    if (last_slash == std::string::npos) {
      base_dir = ".";
      base_name = _base_path;
    } else {
      base_dir = _base_path.substr(0, last_slash);
      base_name = _base_path.substr(last_slash + 1);
    }
  }
  std::list<std::string> parts{
      misc::filesystem::dir_content_with_filter(base_dir, base_name + '*')};
  _rid = std::numeric_limits<int>::max();
  _wid = 0;
  size_t offset{base_dir.size() + base_name.size()};
  if (base_dir.back() != '/')
    offset++;
  for (auto& f : parts) {
    const char* ptr{f.c_str() + offset};
    int val = 0;
    if (*ptr) {  // Not empty, conversion needed.
      char* endptr(nullptr);
      val = strtol(ptr, &endptr, 10);
      if (endptr && *endptr)  // Invalid conversion.
        continue;
    }

    if (val < _rid)
      _rid = val;
    if (val > _wid)
      _wid = val;
  }

  if (_rid == std::numeric_limits<int>::max() || _rid < 0)
    _rid = 0;

  assert(_rid <= _wid);

  _open_write_file();
}

/**
 *  Destructor.
 */
splitter::~splitter() {}

/**
 *  Close files open by splitter.
 *  If no files are open, nothing is done.
 */
void splitter::close() {
  if (_rfile) {
    std::lock_guard<std::mutex> lck(*_rmutex);
    _rfile.reset();
    _rmutex = nullptr;
  }
  if (_wfile) {
    std::lock_guard<std::mutex> lck(*_wmutex);
    _wfile.reset();
    _wmutex = nullptr;
  }
}

/**
 *  Read data.
 *
 *  @param[out] buffer    Output buffer.
 *  @param[in]  max_size  Maximum number of bytes that can be read.
 *
 *  @return Number of bytes read.
 */
long splitter::read(void* buffer, long max_size) {
  if (!_rfile) {
    _open_read_file();
    if (!_rfile)
      return 0;
  }

  std::unique_lock<std::mutex> lck(*_rmutex);
  // Seek to current read position.
  fseek(_rfile.get(), _roffset, SEEK_SET);

  // Read data.
  long rb = fread(buffer, 1, max_size, _rfile.get());
  std::string file_path(get_file_path(_rid));
  logging::debug(logging::low)
      << "file: read " << rb << " bytes from '" << file_path << "'";
  _roffset += rb;
  if (rb == 0) {
    if (feof(_rfile.get())) {
      if (_auto_delete) {
        logging::info(logging::high)
            << "file: end of file '" << file_path << "' reached, eraseing it";
        std::remove(file_path.c_str());
      }
      if (_rid < _wid) {
        _rid++;
        lck.unlock();
        _open_read_file();
        return read(static_cast<char*>(buffer), max_size);
      } else
        throw exceptions::shutdown("No more data to read");
    } else {
      if (errno == EAGAIN || errno == EINTR)
        return 0;
      else
        throw msg_fmt("error while reading file '{}': {}", file_path,
                      strerror(errno));
    }
  }
  return rb;
}

/**
 *  Throw an exception.
 *
 *  @param[in] offset  Unused.
 *  @param[in] whence  Unused.
 */
void splitter::seek(long offset, fs_file::seek_whence whence) {
  (void)offset;
  (void)whence;
  throw msg_fmt("cannot seek within a splitted file");
}

/**
 *  Get current position.
 *
 *  @return Current position in file.
 */
long splitter::tell() {
  return _roffset;
}

/**
 *  Write data.
 *
 *  @param[in] buffer  Data.
 *  @param[in] size    Number of bytes in buffer.
 *
 *  @return Number of bytes written.
 */
long splitter::write(void const* buffer, long size) {
  if (!_wfile)
    _open_write_file();

  {
    std::unique_lock<std::mutex> lck(*_wmutex);
    // Open next write file is max file size is reached.
    if ((_woffset + size) > _max_file_size) {
      ++_wid;
      lck.unlock();
      // After this call, _wmutex may change.
      _open_write_file();
    }
  }
  std::unique_lock<std::mutex> lck(*_wmutex);
  // Otherwise seek to end of file.

  fseek(_wfile.get(), _woffset, SEEK_SET);

  // Debug message.
  logging::debug(logging::low) << "file: write request of " << size
                               << " bytes for '" << get_file_path(_wid) << "'";

  // Write data.
  long remaining = size;
  while (remaining > 0) {
    long wb = fwrite(buffer, 1, remaining, _wfile.get());
    remaining -= wb;
    _woffset += wb;
  }
  return size;
}

/**
 *  Flush the write stream.
 */
void splitter::flush() {
  if (fflush(_wfile.get()) == EOF)
    throw msg_fmt("error while writing the file '{}' content: {}",
                  get_file_path(_wid), strerror(errno));
}

/**
 *  Get the file path matching the ID.
 *
 *  @param[in] id Current ID.
 */
std::string splitter::get_file_path(int id) const {
  if (id)
    return fmt::format("{}{}", _base_path, id);
  else
    return _base_path;
}

/**
 *  Get max file size.
 *
 *  @return Max file size.
 */
long splitter::get_max_file_size() const {
  return _max_file_size;
}

/**
 *  Get current read ID.
 *
 *  @return Current read ID.
 */
int splitter::get_rid() const {
  return _rid;
}

/**
 *  Get current read offset.
 *
 *  @return Current read offset.
 */
long splitter::get_roffset() const {
  return _roffset;
}

/**
 *  Get current write ID.
 *
 *  @return Current write ID.
 */
int splitter::get_wid() const {
  return _wid;
}

/**
 *  Get current write offset.
 *
 *  @return Current write offset.
 */
long splitter::get_woffset() const {
  return _woffset;
}

/**
 *  Remove all the files the splitter is concerned by.
 */
void splitter::remove_all_files() {
  close();
  std::lock_guard<std::mutex> lck1(_mutex1);
  std::lock_guard<std::mutex> lck2(_mutex2);
  std::string base_dir;
  std::string base_name;
  {
    size_t last_slash(_base_path.find_last_of('/'));
    if (last_slash == std::string::npos) {
      base_dir = "./";
      base_name = _base_path;
    } else {
      base_dir = _base_path.substr(0, last_slash + 1);
      base_name = _base_path.substr(last_slash + 1);
    }
  }
  std::list<std::string> parts{
      misc::filesystem::dir_content_with_filter(base_dir, base_name + '*')};
  for (std::string const& f : parts)
    std::remove(f.c_str());
}

/**
 * @brief Open the splitter in read mode.
 */
void splitter::_open_read_file() {
  {
    std::lock_guard<std::mutex> lck(_id_m);
    if (_rid == _wid && _wfile) {
      _rfile = _wfile;
      _rmutex = _wmutex;
    } else {
      std::string fname(get_file_path(_rid));
      FILE* f = fopen(fname.c_str(), "r+");
      _rfile = f ? std::shared_ptr<FILE>(f, fclose) : std::shared_ptr<FILE>();
      if (_rfile)
        _rmutex = &_mutex1;
    }
  }

  if (!_rfile) {
    if (errno == ENOENT)
      return;
    else
      throw msg_fmt("cannot open '{}' to read/write: {}", get_file_path(_rid),
                    strerror(errno));
  }
  std::lock_guard<std::mutex> lck(*_rmutex);
  _roffset = 2 * sizeof(uint32_t);
  fseek(_rfile.get(), _roffset, SEEK_SET);
}

/**
 * @brief Open the splitter in write mode.
 */
void splitter::_open_write_file() {
  {
    std::lock_guard<std::mutex> lck(_id_m);
    if (_wid == _rid && _rfile) {
      _wfile = _rfile;
      _wmutex = _rmutex;
    } else {
      std::string fname(get_file_path(_wid));
      FILE* f = fopen(fname.c_str(), "a+");
      _wfile = f ? std::shared_ptr<FILE>(f, fclose) : std::shared_ptr<FILE>();
      _wmutex = &_mutex2;
    }
  }

  if (!_wfile)
    throw msg_fmt("cannot open '{}' to read/write: {}", get_file_path(_wid),
                  strerror(errno));

  std::lock_guard<std::mutex> lck(*_wmutex);
  fseek(_wfile.get(), 0, SEEK_END);
  _woffset = ftell(_wfile.get());

  // Ensure 8-bytes header is written at file beginning.
  if (_woffset < static_cast<uint32_t>(2 * sizeof(uint32_t))) {
    fseek(_wfile.get(), 0, SEEK_SET);
    union {
      char bytes[2 * sizeof(uint32_t)];
      uint32_t integers[2];
    } header;
    header.integers[0] = 0;
    header.integers[1] = htonl(2 * sizeof(uint32_t));
    size_t size = 0;
    while (size < sizeof(header))
      size +=
          fwrite(header.bytes + size, 1, sizeof(header) - size, _wfile.get());
    _woffset = 2 * sizeof(uint32_t);
  }
}
