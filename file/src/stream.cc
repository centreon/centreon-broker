/*
** Copyright 2011 Merethis
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

#include <assert.h>
#include <QMutexLocker>
#include <QScopedPointer>
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

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
  assert(false);
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
  assert(false);
  abort();
  return (*this);
}

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
  _woffset = _file.pos();
  if (!_file.reset())
    throw (exceptions::msg() << "file: could not reset in '"
             << filename << "': " << _file.errorString());
  _roffset = _file.pos();
  _coffset = _roffset;
  _process_in = (mode & QIODevice::ReadOnly);
  _process_out = (mode & QIODevice::WriteOnly);
}

/**
 *  Destructor.
 */
stream::~stream() {
  logging::debug << logging::MEDIUM << "file: closing '"
    << _file.fileName() << "'";
  _file.flush();
  _file.close();
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
  _process_out = out;
  return ;
}

/**
 *  Read data from the file.
 *
 *  @return Bunch of data.
 */
QSharedPointer<io::data> stream::read() {
  // Lock mutex.
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
  QScopedPointer<io::raw> data(new io::raw);
  data->resize(4096);

  // Read data.
  QSharedPointer<io::data> retval;
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
    data->resize(rb);
    _roffset += rb;
    _coffset = _roffset;
    retval = QSharedPointer<io::data>(data.take());
  }
  return (retval);
}

/**
 *  Write data to the file.
 *
 *  @param[in] d Data to write.
 */
void stream::write(QSharedPointer<io::data> d) {
  // Check that writing should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "file stream is shutdown");

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
