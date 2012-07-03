/*
** Copyright 2011-2012 Merethis
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

#include <cassert>
#include <cstdlib>
#include <memory>
#include <QMutexLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/stream.hh"
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
 *  @param[in] filename Filename.
 *  @param[in] mode     Open mode.
 */
stream::stream(QString const& filename, QIODevice::OpenMode mode)
  : _file(filename) {
  if (!_file.open(mode))
    throw (exceptions::msg() << "file: could not open '"
             << filename << "': " << _file.errorString());
  if (!_file.reset())
    throw (exceptions::msg() << "file: could not reset in '"
             << filename << "': " << _file.errorString());
  _roffset = _file.pos();
  _coffset = _roffset;
  _woffset = _roffset + _file.size();
  _process_in = (mode & QIODevice::ReadOnly);
  _process_out = (mode & QIODevice::WriteOnly);
}

/**
 *  Destructor.
 */
stream::~stream() {
  logging::debug(logging::medium) << "file: closing '"
    << _file.fileName() << "'";
  _file.flush();
  _file.close();
  if (_woffset == _roffset) {
    logging::info(logging::high) << "file: end of file '"
      << _file.fileName() << "' reached, erasing file";
    QFile::remove(_file.fileName());
  }
  else
    logging::debug(logging::medium) << "file: not erasing '"
      << _file.fileName() << "' write offset is " << _woffset
      << " whereas read offset is " << _roffset;
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
  if (!_process_in)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "file stream is shutdown");

  // Seek if necessary.
  if (_roffset != _coffset) {
    if (!_file.seek(_roffset))
      throw (exceptions::msg() << "file: could not seek for reading " \
                  "in '" << _file.fileName() << "' at offset "
               << static_cast<unsigned long long>(_roffset));
  }

  // Build data array.
  std::auto_ptr<io::raw> data(new io::raw);
  data->resize(4096);

  // Read data.
  qint64 old_roffset(_roffset);
  qint64 rb(_file.read(data->QByteArray::data(), data->size()));
  if (!rb) // XXX : io::exceptions::error
    throw (io::exceptions::shutdown(true, !_process_out)
             << "file does not have any more data");
  else if (rb < 0) {
    exceptions::msg e;
    e << "file: could not read data from '"
      << _file.fileName() << "': " << _file.errorString();
    _roffset = _file.pos();
    _coffset = _roffset;
    throw (e);
  }
  else {
    // Process data.
    data->resize(rb);
    _roffset += rb;
    _coffset = _roffset;
    d = misc::shared_ptr<io::data>(data.release());

    // Erase read data.
    if (!_file.seek(old_roffset))
      logging::error(logging::medium) << "file: erase seek failed in '"
        << _file.fileName() << "'";
    else {
      QByteArray eraser;
      eraser.fill('\n', rb);
      while (!eraser.isEmpty()) {
        rb = _file.write(eraser.data(), eraser.size());
        if (rb <= 0) {
          logging::error(logging::medium)
            << "file: erasing request failed in '" << _file.fileName()
            << "'";
          _coffset -= eraser.size();
          break ;
        }
        else
          eraser.resize(eraser.size() - rb);
      }
    }
  }
  return ;
}

/**
 *  Write data to the file.
 *
 *  @param[in] d Data to write.
 */
void stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data exists and should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "file stream is shutdown");
  if (d.isNull())
    return ;

  if (d->type() == "com::centreon::broker::io::raw") {
    // Lock mutex.
    QMutexLocker lock(&_mutex);

    // Seek to end of file if necessary.
    if (_woffset != _coffset) {
      if (!_file.seek(_woffset))
        throw (exceptions::msg() << "file: could not seek for writing" \
                    " in '" << _file.fileName() << "' at offset "
                 << static_cast<unsigned long long>(_woffset));
    }

    // Get data.
    void* memory;
    unsigned int size;
    {
      io::raw* data(static_cast<io::raw*>(d.data()));
      memory = data->QByteArray::data();
      size = data->size();
    }

    // Debug message.
    logging::debug(logging::low) << "file: write request of "
      << size << " bytes";

    // Write data.
    while (size > 0) {
      qint64 wb(_file.write(static_cast<char*>(memory), size));
      if (wb <= 0) {
        exceptions::msg e;
        e << "file: could not write data in '" << _file.fileName()
          << "': " << _file.errorString();
        _woffset = _file.pos();
        _coffset = _woffset;
        throw (e);
      }
      size -= wb;
      _woffset += wb;
      _file.waitForBytesWritten(-1);
    }
    _coffset = _woffset;
  }
  else
    logging::info(logging::low) << "file: write request with "	\
      "invalid data (" << d->type() << ")";
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will lead to a call to abort().
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  assert(!"file stream is not copyable");
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will lead to a call to abort().
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  (void)s;
  assert(!"file stream is not copyable");
  abort();
  return (*this);
}
