/*
** Copyright 2012,2016-2017 Centreon
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

#include "com/centreon/broker/file/cfile.hh"
#include <cerrno>
#include <cstring>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"

using namespace com::centreon::broker::file;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Open a file.
 *
 *  @param[in] path  Path to file.
 *  @param[in] mode  Open mode.
 */
cfile::cfile(std::string const& path, fs_file::open_mode mode)
    : _stream(nullptr), _path(path), _mode(mode) {
  _open();
}

/**
 *  Destructor.
 */
cfile::~cfile() {
  close();
}

/**
 *  Open the file following mode given in constructor.
 */
void cfile::_open() {
  // Compute cfile's mode.
  char const* cfile_mode;
  switch (_mode) {
    case fs_file::open_write:
      cfile_mode = "w";
      break;
    case fs_file::open_read_write_truncate:
      cfile_mode = "w+";
      break;
    case fs_file::open_read_write_no_create:
      cfile_mode = "r+";
      break;
    default:
      cfile_mode = "r";
  };

  // Open file.
  _stream = fopen(_path.c_str(), cfile_mode);
  if (!_stream) {
    char const* msg(strerror(errno));
    throw(exceptions::msg() << "cannot open '" << _path << "' (mode "
                            << cfile_mode << "): " << msg);
  }
}

/**
 *  Close the file.
 */
void cfile::close() {
  if (_stream) {
    fclose(_stream);
    _stream = nullptr;
  }
}

/**
 *  Read data from file.
 *
 *  @param[out] buffer   Destination buffer.
 *  @param[in]  max_size Maximum size in bytes to read.
 *
 *  @return Number of bytes read.
 */
long cfile::read(void* buffer, long max_size) {
  if (!_stream)
    _open();
  size_t retval(fread(buffer, 1, max_size, _stream));
  if (retval == 0) {
    if (feof(_stream))
      throw(exceptions::shutdown() << "end of file reached");
    else if ((EAGAIN == errno) || (EINTR == errno))
      retval = 0;
    else {
      char const* msg(strerror(errno));
      throw(exceptions::msg() << "error while reading file: " << msg);
    }
  }
  return (retval);
}

/**
 *  Seek to a specific position.
 *
 *  @param[in] offset Offset.
 *  @param[in] whence Base position.
 */
void cfile::seek(long offset, fs_file::seek_whence whence) {
  if (!_stream)
    _open();
  // Compute cfile's whence.
  int seek_whence;
  switch (whence) {
    case fs_file::seek_current:
      seek_whence = SEEK_CUR;
      break;
    case fs_file::seek_end:
      seek_whence = SEEK_END;
      break;
    default:
      seek_whence = SEEK_SET;
  };

  // Seek.
  int retval;
  while ((retval = fseek(_stream, offset, seek_whence)) && (EAGAIN == errno) &&
         (EINTR == errno))
    ;
  if (retval) {
    char const* msg(strerror(errno));
    throw(exceptions::msg() << "cannot seek in file to position ("
                            << seek_whence << ", " << offset << "): " << msg);
  }

  return;
}

/**
 *  Return the current position in the file.
 *
 *  @return Current offset in the file.
 */
long cfile::tell() {
  if (!_stream)
    _open();
  long retval(ftell(_stream));
  if (-1 == retval) {
    char const* msg(strerror(errno));
    throw(exceptions::msg() << "cannot tell position in file: " << msg);
  }
  return (retval);
}

/**
 *  Write data to the file.
 *
 *  @param[in] buffer Data.
 *  @param[in] size   Size of data.
 *
 *  @return Number of bytes written.
 */
long cfile::write(void const* buffer, long size) {
  if (!_stream)
    _open();
  size_t retval(fwrite(buffer, 1, size, _stream));
  if (ferror(_stream)) {
    char const* msg(strerror(errno));
    throw(exceptions::msg()
          << "cannot write " << size << " bytes to file: " << msg);
  }
  return (retval);
}

/**
 *  Create a new cfile.
 *
 *  @param[in] path  Path to file.
 *  @param[in] mode  Open mode.
 *
 *  @return A new cfile object.
 */
cfile* cfile_factory::new_cfile(std::string const& path,
                                fs_file::open_mode mode) {
  return (new cfile(path, mode));
}

/**
 *  Create a new cfile.
 *
 *  @param[in] path  Path to file.
 *  @param[in] mode  Open mode.
 *
 *  @return A new cfile object.
 */
fs_file* cfile_factory::new_fs_file(std::string const& path,
                                    fs_file::open_mode mode) {
  return (new_cfile(path, mode));
}
