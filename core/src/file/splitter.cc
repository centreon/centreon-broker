/*
** Copyright 2017 Centreon
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

#include <arpa/inet.h>
#include <cstdlib>
#include <limits>
#include <memory>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/cfile.hh"
#include "com/centreon/broker/file/qt_fs_browser.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**
 *  Build a new splitter.
 *
 *  @param[in] path           Base path to file.
 *  @param[in] mode           Ignored. Files will always be open
 *                            read/write.
 *  @param[in] file_factory   fs_file_factory to work with single files.
 *  @param[in] fs             fs_browser object to manipulate file
 *                            system.
 *  @param[in] max_file_size  Maximum single file size.
 *  @param[in] auto_delete    True to delete file parts as they are
 *                            read.
 */
splitter::splitter(
            std::string const& path,
            fs_file::open_mode mode,
            fs_file_factory* file_factory,
            fs_browser* fs,
            long max_file_size,
            bool auto_delete)
  : _auto_delete(auto_delete),
    _base_path(path),
    _file_factory(file_factory),
    _fs(fs),
    _max_file_size(max_file_size),
    _rid(0),
    _roffset(0),
    _wid(0),
    _woffset(0) {
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
    }
    else {
      base_dir = _base_path.substr(0, last_slash).c_str();
      base_name = _base_path.substr(last_slash + 1).c_str();
    }
  }
  fs_browser::entry_list parts;
  {
    std::string name_pattern(base_name);
    name_pattern.append("*");
    parts = _fs->read_directory(base_dir, name_pattern);
  }
  _rid = std::numeric_limits<int>::max();
  _wid = 0;
  for (fs_browser::entry_list::iterator
         it(parts.begin()),
         end(parts.end());
       it != end;
       ++it) {
    char const* ptr(it->c_str() + base_name.size());
    int val(0);
    if (*ptr) { // Not, empty, conversion needed.
      char* endptr(NULL);
      val = strtol(ptr, &endptr, 10);
      if (endptr && *endptr) // Invalid conversion.
        continue ;
    }

    if (val < _rid)
      _rid = val;
    if (val > _wid)
      _wid = val;
  }
  if ((_rid == std::numeric_limits<int>::max())
      || (_rid < 0))
    _rid = 0;

  // Initial write file opening to allow read file to be opened
  // with no exception.
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
  if (!_rfile.isNull()) {
    _rfile->close();
    _rfile.clear();
  }
  if (!_wfile.isNull()) {
    _wfile->close();
    _wfile.clear();
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
  // Open next file if necessary.
  if (_rfile.isNull())
    _open_read_file();
  // Otherwise seek to current read position.
  else
    _rfile->seek(_roffset);

  // Read data.
  try {
    long rb(_rfile->read(buffer, max_size));
    logging::debug(logging::low) << "file: read " << rb << " bytes from '"
      << get_file_path(_rid) << "'";
    _roffset += rb;
    return (rb);
  }
  catch (exceptions::shutdown const& e) {
    (void)e;

    // Erase file that just got read.
    bool reached_end(_wid == _rid);
    _rfile.clear();
    if (reached_end)
      _wfile.clear();
    std::string file_path(get_file_path(_rid));
    if (_auto_delete) {
      logging::info(logging::high) << "file: end of file '"
        << file_path << "' reached, erasing file";
      _fs->remove(file_path.c_str());
    }
    else {
      logging::info(logging::high) << "file: end of file '"
        << file_path << "' reached, NOT erasing file";
    }

    // The current read position reached the write position.
    // Reading is over.
    if (reached_end)
      throw ;
    // Open next read file.
    else {
      ++_rid;
      _open_read_file();
      return (read(buffer, max_size));
    }
  }
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
  throw (exceptions::msg() << "cannot seek within a splitted file");
}

/**
 *  Get current position.
 *
 *  @return Current position in file.
 */
long splitter::tell() {
  return (_roffset);
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
  // Open current write file if not already done.
  if (_wfile.isNull())
    _open_write_file();
  // Open next write file is max file size is reached.
  else if ((_woffset + size) > _max_file_size) {
    _wfile.clear();
    ++_wid;
    _open_write_file();
  }
  // Otherwise seek to end of file.
  else
    _wfile->seek(_woffset);

  // Debug message.
  logging::debug(logging::low) << "file: write request of "
    << size << " bytes for '" << get_file_path(_wid) << "'";

  // Write data.
  long remaining(size);
  while (remaining > 0) {
    long wb(_wfile->write(buffer, remaining));
    remaining -= wb;
    _woffset += wb;
    buffer = static_cast<char const*>(buffer) + wb;
  }

  return (size);
}

/**
 *  Get the file path matching the ID.
 *
 *  @param[in] id Current ID.
 */
std::string splitter::get_file_path(int id) const {
  if (id) {
    std::ostringstream oss;
    oss << _base_path << id;
    return (oss.str());
  }
  else
    return (_base_path);
}

/**
 *  Get max file size.
 *
 *  @return Max file size.
 */
long splitter::get_max_file_size() const {
  return (_max_file_size);
}

/**
 *  Get current read ID.
 *
 *  @return Current read ID.
 */
int splitter::get_rid() const {
  return (_rid);
}

/**
 *  Get current read offset.
 *
 *  @return Current read offset.
 */
long splitter::get_roffset() const {
  return (_roffset);
}

/**
 *  Get current write ID.
 *
 *  @return Current write ID.
 */
int splitter::get_wid() const {
  return (_wid);
}

/**
 *  Get current write offset.
 *
 *  @return Current write offset.
 */
long splitter::get_woffset() const {
  return (_woffset);
}

/**
 *  Remove all the files the splitter is concerned by.
 */
void splitter::remove_all_files() {
  close();
  std::string base_dir;
  std::string base_name;
  {
    size_t last_slash(_base_path.find_last_of('/'));
    if (last_slash == std::string::npos) {
      base_dir = "./";
      base_name = _base_path;
    }
    else {
      base_dir = _base_path.substr(0, last_slash + 1).c_str();
      base_name = _base_path.substr(last_slash + 1).c_str();
    }
  }
  fs_browser::entry_list parts;
  {
    std::string name_pattern(base_name);
    name_pattern.append("*");
    parts = _fs->read_directory(base_dir, name_pattern);
  }
  for (fs_browser::entry_list::iterator it(parts.begin()), end(parts.end());
       it != end;
       ++it)
    _fs->remove(base_dir + '/' + *it);
}

/**
 *  Open the readable file.
 */
void splitter::_open_read_file() {
  _rfile.clear();

  // If we reached write-ID and wfile is open, use it.
  if ((_rid == _wid) && !_wfile.isNull())
    _rfile = _wfile;
  // Otherwise open next file.
  else {
    std::string file_path(get_file_path(_rid));
    misc::shared_ptr<fs_file>
      new_file(_file_factory->new_fs_file(
                                file_path,
                                fs_file::open_read_write_no_create));
    _rfile = new_file;
  }
  _roffset = 2 * sizeof(uint32_t);
  _rfile->seek(_roffset);

  return ;
}

/**
 *  Open the writable file.
 */
void splitter::_open_write_file() {
  _wfile.clear();

  // If we are already reading the latest file, use it.
  if ((_rid == _wid) && !_rfile.isNull())
    _wfile = _rfile;
  // Otherwise open file.
  else {
    std::string file_path(get_file_path(_wid));
    logging::info(logging::high) << "file: opening new file '"
      << file_path.c_str() << "'";
    try {
      _wfile = _file_factory->new_fs_file(
                                file_path,
                                fs_file::open_read_write_no_create);
    }
    catch (exceptions::msg const& e) {
      _wfile = _file_factory->new_fs_file(
                                file_path,
                                fs_file::open_read_write_truncate);
    }
  }

  // Position.
  _wfile->seek(0, fs_file::seek_end);
  _woffset = _wfile->tell();

  // Ensure 8-bytes header is written at file beginning.
  if (_woffset < static_cast<long>(2 * sizeof(uint32_t))) {
    _wfile->seek(0);
    union {
      char     bytes[2 * sizeof(uint32_t)];
      uint32_t integers[2];
    } header;
    header.integers[0] = 0;
    header.integers[1] = htonl(2 * sizeof(uint32_t));
    unsigned int size(0);
    while (size < sizeof(header))
      size += _wfile->write(header.bytes + size, sizeof(header) - size);
    _woffset = 2 * sizeof(uint32_t);
  }

  return ;
}

/**
 *  Build a new default splitter.
 *
 *  @param[in] path  Path to file.
 *  @param[in] mode  Open mode (ignored).
 *
 *  @return A new default splitter.
 */
fs_file* splitter_factory::new_fs_file(
                             std::string const& path,
                             fs_file::open_mode mode) {
  return (new_cfile_splitter(path, mode));
}

/**
 *  Build a new cfile splitter.
 *
 *  @param[in] path           Path to file.
 *  @param[in] mode           Open mode (ignored).
 *  @param[in] max_file_size  Max single file size.
 *  @param[in] auto_delete    True to delete file parts as they are
 *                            read.
 *
 *  @return A new cfile splitter.
 */
splitter* splitter_factory::new_cfile_splitter(
                              std::string const& path,
                              fs_file::open_mode mode,
                              long max_file_size,
                              bool auto_delete) {
  std::unique_ptr<fs_file_factory> f(new cfile_factory());
  std::unique_ptr<fs_browser> b(new qt_fs_browser());
  splitter* s(new splitter(
                    path,
                    mode,
                    f.release(),
                    b.release(),
                    max_file_size,
                    auto_delete));
  return (s);
}
