/*
** Copyright 2011-2014 Merethis
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
  : _max_size(max_size),
    _path(path),
    _process_in(true),
    _process_out(true) {
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
 *  Set processing flags.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void stream::process(bool in, bool out) {
  QMutexLocker lock(&_mutex);
  _process_in = in;
  _process_out = in || !out;
  return ;
}

/**
 *  Read data from the file.
 *
 *  @param[out] d Bunch of data.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  // Lock mutex.
  d.clear();
  QMutexLocker lock(&_mutex);

  // Check that read should be done.
  if (!_process_in || !_rfile.data())
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "file stream is shutdown");

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
      std::string file_path(_file_path(_rid));
      logging::info(logging::high) << "file: end of last file '"
        << file_path.c_str() << "' reached, closing and erasing file";
      ::remove(file_path.c_str());
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
  return ;
}

/**
 *  Generate statistics about file processing.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(std::string& buffer) const {
  std::ostringstream oss;
  oss << "file_read_path=" << _file_path(_rid) << "\n"
      << "file_read_offset=" << _roffset << "\n"
      << "file_write_path=" << _file_path(_wid) << "\n"
      << "file_write_offset=" << _woffset << "\n"
      << "file_max_size=";
  if (_max_size != std::numeric_limits<long>::max())
    oss << _max_size;
  else
    oss << "unlimited";
  oss << "\n";
  oss << "file_percent_processed="
      << std::fixed << std::setprecision(1);
  if ((_rid != _wid)
      && (_max_size == std::numeric_limits<long>::max()))
    oss << "unknown\n";
  else
    oss << (_roffset * 100.0)
           / (_woffset + (_wid - _rid) * _max_size) << "%\n";
  buffer.append(oss.str());
  return ;
}

/**
 *  Write data to the file.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged (1).
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data exists and should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "file stream is shutdown");
  if (d.isNull())
    return (1);

  if (d->type() == io::events::data_type<io::events::internal, 1>::value) {
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

    // Write data.
    while (size > 0) {
      if (_woffset == _max_size)
        _open_next_write();
      unsigned long max_write(_max_size - _woffset);
      if (size < max_write)
        max_write = size;
      unsigned long
        wb(_wfile->write(memory, max_write));
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
  logging::info(logging::high) << "file: end of file '"
    << file_path.c_str() << "' reached, erasing file";
  ::remove(file_path.c_str());

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
