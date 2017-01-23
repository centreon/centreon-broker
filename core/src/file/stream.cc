/*
** Copyright 2011-2017 Centreon
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
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <memory>
#include <QDir>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] filename     Filename.
 *  @param[in] mode         Open mode.
 *  @param[in] is_temporary Create temporary path base on the filename.
 */
stream::stream(
          std::string const& path,
          unsigned long long max_size)
  : _auto_delete(true),
    _last_read_offset(0),
    _last_time(0),
    _last_write_offset(0),
    _max_size(max_size),
    _path(path) {
  if ((max_size <= 2 * sizeof(uint32_t))
      || (max_size > static_cast<unsigned long long>(
                       std::numeric_limits<long>::max())))
    _max_size = std::numeric_limits<long>::max();
  _open_first_write();
  _open_first_read();
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  @brief Get stream's maximum size.
 *
 *  This size is used to determine the maximum file size that a file can
 *  have. After this limit, the next file (<file>, <file>1, <file>2,
 *  ...) will be opened.
 *
 *  @return Max size in bytes.
 */
unsigned long long stream::get_max_size() const throw () {
  return (_max_size);
}

/**
 *  Read data from the file.
 *
 *  @param[out] d         Bunch of data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Always true as file never times out.
 */
bool stream::read(
               misc::shared_ptr<io::data>& d,
               time_t deadline) {
  (void)deadline;

  // Lock mutex.
  d.clear();
  QMutexLocker lock(&_mutex);

  // Check that read should be done.
  if (!_rfile.data())
    throw (io::exceptions::shutdown(true, false)
           << "end of file");

  // Seek to position.
  _rfile->seek(_roffset);

  // Build data array.
  std::auto_ptr<io::raw> data(new io::raw);
  data->resize(BUFSIZ);

  // Read data.
  unsigned long rb;
  try {
    rb = _rfile->read(data->QByteArray::data(), data->size());
  }
  catch (io::exceptions::shutdown const& e) {
    (void)e;
    if (_wid == _rid) {
      _rfile->close();
      _rfile.clear();
      _wfile.clear();
      std::string file_path(_file_path(_rid));
      if (_auto_delete) {
        logging::info(logging::high) << "file: end of last file '"
          << file_path.c_str() << "' reached, closing and erasing file";
        ::remove(file_path.c_str());
      }
      else {
        logging::info(logging::high) << "file: end of last file '"
          << file_path.c_str() << "' reached, closing but NOT erasing file";
      }
      throw ;
    }
    _open_next_read();
    rb = _rfile->read(data->QByteArray::data(), data->size());
  }

  // Process data.
  logging::debug(logging::low) << "file: read " << rb << " bytes from '"
    << _file_path(_rid).c_str() << "'";
  data->resize(rb);
  _roffset += rb;
  d = misc::shared_ptr<io::data>(data.release());

  // Erase read data.
  _rfile->seek(0);
  union {
    char     bytes[2 * sizeof(uint32_t)];
    uint32_t integers[2];
  } header;
  header.integers[0] = htonl(_roffset / 4294967296ull);
  header.integers[1] = htonl(_roffset % 4294967296ull);
  unsigned int written(0);
  while (written != sizeof(header.bytes))
    written += _rfile->write(
                         header.bytes + written,
                         sizeof(header.bytes) - written);
return (true);
}

/**
 *  Reset read file to the beginning.
 */
void stream::reset() {
  // XXX
}

/**
 *  @brief Set auto-delete mode.
 *
 *  In auto-delete mode, file parts are deleted as they are read.
 */
void stream::set_auto_delete(bool auto_delete) {
  _auto_delete = auto_delete;
  return ;
}

/**
 *  Generate statistics about file processing.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(io::properties& tree) const {
  std::ostringstream oss;
  // Easy to print.
  {
    tree.add_property(
           "file_read_path",
           io::property("file_read_path", misc::string::get(_rid)));
  }
  {
    tree.add_property(
           "file_read_offset",
           io::property("file_read_offset", misc::string::get(_roffset)));
  }
  {
    tree.add_property(
           "file_write_path",
           io::property("file_write_path", misc::string::get(_wid)));
  }
  {
    tree.add_property(
           "file_write_offset",
           io::property("file_write_offset", misc::string::get(_woffset)));
  }
  {
    tree.add_property(
           "file_max_size",
           io::property(
                 "file_max_size",
                 _max_size != std::numeric_limits<long>::max()
                 ? misc::string::get(_max_size)
                 : "unlimited"));
  }

  // Need computation.
  bool write_time_expected(false);
  {
    io::property& p(tree["file_percent_processed"]);
    p.set_name("file_percent_processed");
    oss.str("");
    if (_rid != _wid
        && _max_size == std::numeric_limits<long>::max()) {
      oss << "unknown";
    }
    else {
      oss << (_roffset * 100.0) / (_woffset + (_wid - _rid) * _max_size)
          << "%";
      write_time_expected = true;
    }
    p.set_value(oss.str());
  }
  if (write_time_expected) {
    time_t now(time(NULL));
    unsigned long long roffset(_roffset + _rid * _max_size);
    unsigned long long woffset(_woffset + _wid * _max_size);

    if (_last_time && (now != _last_time)) {
      time_t eta(0);
      {
        io::property& p(tree["file_expected_terminated_at"]);
        oss.str("");
        p.set_name("file_expected_terminated_at");

        unsigned long long
          div(roffset + _last_write_offset - _last_read_offset - woffset);
        if (div == 0)
          oss << "file not processed fast enough to terminate";
        else {
          eta = now + (woffset - roffset) * (now - _last_time) / div;
          oss << eta;
        }
        p.set_value(oss.str());
      }

      if (_max_size == std::numeric_limits<long>::max()) {
        io::property& p(tree["file_expected_max_size"]);
        oss.str("");
        p.set_name("file_expected_max_size");
        oss << woffset
               + (woffset - _last_write_offset)
               * (eta - now)
               / (now - _last_time);

        p.set_value(oss.str());
        p.set_graphable(false);
      }
    }

    _last_time = now;
    _last_read_offset = roffset;
    _last_write_offset = woffset;
  }

  return ;
}

/**
 *  Write data to the file.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged (1).
 */
int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data exists.
  if (!validate(d, "file"))
    return (1);

  // Check that file should still be written to.
  if (_wfile.isNull())
    throw (io::exceptions::shutdown(true, true)
           << "end of file");

  if (d->type() == io::raw::static_type()) {
    // Lock mutex.
    QMutexLocker lock(&_mutex);

    // Seek to end of file if necessary.
    _wfile->seek(_woffset);

    // Get data.
    char const* memory;
    unsigned int size;
    {
      io::raw* data(static_cast<io::raw*>(d.data()));
      memory = data->QByteArray::data();
      size = data->size();
    }

    // Debug message.
    logging::debug(logging::low) << "file: write request of "
      << size << " bytes for '" << _file_path(_wid).c_str() << "'";

    // Open new file if necessary.
    if ((_woffset + size) > _max_size)
      _open_next_write();

    // Write data.
    while (size > 0) {
      unsigned long
        wb(_wfile->write(memory, size));
      size -= wb;
      _woffset += wb;
      memory += wb;
    }
  }
  else
    logging::info(logging::low) << "file: write request with "	\
      "invalid data (" << d->type() << ")";
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Get the file path matching the ID.
 *
 *  @param[in] id Current ID.
 */
std::string stream::_file_path(unsigned int id) const {
  if (id) {
    std::ostringstream oss;
    oss << _path << id;
    return (oss.str());
  }
  return (_path);
}

/**
 *  Open the first readable file.
 */
void stream::_open_first_read() {
  // Get path components.
  QString base_dir;
  QString base_name;
  {
    size_t last_slash(_path.find_last_of('/'));
    if (last_slash == std::string::npos) {
      base_dir = ".";
      base_name = _path.c_str();
    }
    else {
      base_dir = _path.substr(0, last_slash).c_str();
      base_name = _path.substr(last_slash + 1).c_str();
    }
  }

  // Browse directory.
  QStringList entries;
  {
    QStringList filters;
    filters << base_name + "*";
    QDir dir(base_dir);
    entries = dir.entryList(filters);
  }

  // Find minimum value.
  unsigned int min(UINT_MAX);
  for (QStringList::iterator it(entries.begin()), end(entries.end());
       it != end;
       ++it) {
    it->remove(0, base_name.size());
    unsigned int i(it->toUInt());
    if (i < min)
      min = i;
  }

  // If no file was found this is an error.
  if (UINT_MAX == min)
    throw (io::exceptions::shutdown(true, true)
           << "cannot find file entry in '" << qPrintable(base_dir)
           << "' matching '" << qPrintable(base_name) << "'");

  // Open file.
  _rid = min - 1;
  _open_next_read();

  return ;
}

/**
 *  Open the first writable file.
 */
void stream::_open_first_write() {
  // Get path components.
  QString base_dir;
  QString base_name;
  {
    size_t last_slash(_path.find_last_of('/'));
    if (last_slash == std::string::npos) {
      base_dir = ".";
      base_name = _path.c_str();
    }
    else {
      base_dir = _path.substr(0, last_slash).c_str();
      base_name = _path.substr(last_slash + 1).c_str();
    }
  }

  // Browse directory.
  QStringList entries;
  {
    QStringList filters;
    filters << base_name + "*";
    QDir dir(base_dir);
    entries = dir.entryList(filters);
  }

  // Find maximum value.
  unsigned int max(0);
  for (QStringList::iterator it(entries.begin()), end(entries.end());
       it != end;
       ++it) {
    it->remove(0, base_name.size());
    unsigned int i(it->toUInt());
    if (i > max)
      max = i;
  }
  _wid = max - 1;

  // Open file.
  _open_next_write(false);

  return ;
}

/**
 *  Open the next readable file.
 */
void stream::_open_next_read() {
  // Did we reached the write file ?
  if (_rid + 1 == _wid) {
    _rfile = _wfile;
    _rfile->seek(0);
  }
  else {
    // Open file.
    std::string file_path(_file_path(_rid + 1));
    {
      misc::shared_ptr<cfile> new_file(new cfile);
      new_file->open(file_path.c_str(), "r+");
      _rfile = new_file;
    }
  }

  // Remove previous file.
  std::string file_path(_file_path(_rid));
  if (_auto_delete) {
    logging::info(logging::high) << "file: end of file '"
      << file_path.c_str() << "' reached, erasing file";
    ::remove(file_path.c_str());
  }
  else {
    logging::info(logging::high) << "file: end of file '"
      << file_path.c_str() << "' reached, NOT erasing file";
  }

  // Adjust current index.
  ++_rid;

  // Get read offset.
  union {
    char bytes[2 * sizeof(uint32_t)];
    uint32_t integers[2];
  } header;
  unsigned int size(0);
  while (size != sizeof(header))
    size += _rfile->read(header.bytes + size, sizeof(header) - size);
  _roffset = ntohl(header.integers[0]) * 4294967296ull
             + ntohl(header.integers[1]);

  return ;
}

/**
 *  Open the next writable file.
 *
 *  @param[in] truncate true to truncate file.
 */
void stream::_open_next_write(bool truncate) {
  // Open file.
  std::string file_path(_file_path(_wid + 1));
  logging::info(logging::high) << "file: opening new file '"
    << file_path.c_str() << "'";
  {
    misc::shared_ptr<cfile> new_file(new cfile);
    if (truncate)
      new_file->open(file_path.c_str(), "w+");
    else {
      try {
        new_file->open(file_path.c_str(), "r+");
      }
      catch (exceptions::msg const& e) {
        new_file->open(file_path.c_str(), "w+");
      }
    }
    _wfile = new_file;
  }

  // Position.
  _wfile->seek(0, SEEK_END);
  _woffset = _wfile->tell();

  // Adjust current index.
  ++_wid;

  if (_woffset < static_cast<long>(2 * sizeof(uint32_t))) {
    // Rewind to file beginning.
    _wfile->seek(0);

    // Write read offset.
    union {
      char     bytes[2 * sizeof(uint32_t)];
      uint32_t integers[2];
    } header;
    header.integers[0] = 0;
    header.integers[1] = htonl(2 * sizeof(uint32_t));
    unsigned int size(0);
    while (size < sizeof(header))
      size += _wfile->write(header.bytes + size, sizeof(header) - size);

    // Set current offset.
    _woffset = 2 * sizeof(uint32_t);
  }

  return ;
}
