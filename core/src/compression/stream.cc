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

#include <zlib.h>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/exceptions/interrupt.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/exceptions/timeout.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

/**
 * Compression function
 *
 * @param data the data to compress.
 * @param compression_level The compression level, by default -1.
 *
 * @return The same data compressed.
 */
static QByteArray _compress(QByteArray const& data, int compression_level) {
  if (data.isEmpty())
    return QByteArray(4, '\0');

  int nbytes = data.size();

  if (compression_level < -1 || compression_level > 9)
    compression_level = -1;

  unsigned long len = nbytes + nbytes / 100 + 13;
  QByteArray retval;
  int res;
  do {
    retval.resize(len + 4);
    res = ::compress2(
        (uchar*)retval.data() + 4,
        &len,
        (uchar*)data.constData(),
        nbytes,
        compression_level);

    switch (res) {
      case Z_OK:
        retval.resize(len + 4);
        retval[0] = (nbytes >> 24) & 0xff;
        retval[1] = (nbytes >> 16) & 0xff;
        retval[2] = (nbytes >> 8) & 0xff;
        retval[3] = (nbytes & 0xff);
        break;
      case Z_MEM_ERROR:
        logging::error(logging::medium)
          << "compression : Z_MEM_ERROR: Not enough memory";
        retval.resize(0);
        break;
      case Z_BUF_ERROR:
        len <<= 1;
        break;
    }
  } while (res == Z_BUF_ERROR);

  return retval;
}

/**
 * Uncompress function
 *
 * @param data The data to extract.
 * @param nbytes The data size in bytes.
 *
 * @return the extract data
 */
static QByteArray _uncompress(const uchar* data, int nbytes) {
  if (!data) {
    logging::info(logging::medium)
      << "compression: uncompress - Data is null";
    return QByteArray();
  }
  if (nbytes <= 4) {
    if (nbytes < 4 || (data[0] != 0 || data[1] != 0
                   || data[2] != 0 || data[3] != 0))
      logging::error(logging::medium)
        << "compression: uncompress - Input data is corrupted";
    return QByteArray();
  }
  ulong expected_size = (data[0] << 24) | (data[1] << 16)
                     | (data[2] <<  8) | data[3];
  ulong len = qMax(expected_size, 1ul);
  QByteArray uncompressed_array(len, '\0');

  forever {
    ulong alloc = len;
    uncompressed_array.resize(alloc);

    int res = ::uncompress((uchar*)uncompressed_array.data(), &len,
        (uchar*)data + 4, nbytes - 4);

    switch (res) {
      case Z_OK:
        if (len != alloc)
          uncompressed_array.resize(len);

        return uncompressed_array;
      case Z_MEM_ERROR:
        logging::error(logging::medium)
          << "compression: uncompress - Z_MEM_ERROR: Not enough memory";
        return QByteArray();
      case Z_BUF_ERROR:
        len <<= 1;
        continue;
      case Z_DATA_ERROR:
        logging::error(logging::medium)
          << "compression: uncompress - Z_DATA_ERROR: Input data is corrupted";
        return QByteArray();
    }
  }
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] level Compression level.
 *  @param[in] size  Compression buffer size.
 */
stream::stream(int level, int size)
  : _level(level), _shutdown(false), _size(size) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
stream::stream(stream const& other) : io::stream(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
stream::~stream() {
  try {
    _flush();
  }
  // Ignore exception whatever the error might be.
  catch (...) {}
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& other) {
  if (this != &other) {
    io::stream::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Read data.
 *
 *  @param[out] data      Data packet.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool stream::read(
               misc::shared_ptr<io::data>& data,
               time_t deadline) {
  // Clear existing content.
  data.clear();

  try {
    // Process buffer as long as data is corrupted
    // or until an exception occurs.
    bool corrupted(true);
    int size(0);
    int skipped(0);
    while (corrupted) {
      // Get compressed data length.
      while (corrupted) {
        _get_data(sizeof(qint32), deadline);

        // We do not have enough data to get the next chunk's size.
        // Stream is shutdown.
        if (_rbuffer.size() < static_cast<int>(sizeof(qint32)))
          throw (exceptions::shutdown() << "no more data to uncompress");

        // Extract next chunk's size.
        {
          unsigned char const* buff((unsigned char const*)_rbuffer.data());
          size = (buff[0] << 24)
                  | (buff[1] << 16)
                  | (buff[2] << 8)
                  | (buff[3]);
        }

        // Check if size is within bounds.
        if ((size <= 0) || (size > max_data_size)) {
          // Skip corrupted data, one byte at a time.
          logging::error(logging::low)
            << "compression: " << this
            << " got corrupted packet size of " << size
            << " bytes, not in the 0-" << max_data_size
            << " range, skipping next byte";
          if (!skipped)
            logging::error(logging::high) << "compression: peer "
              << peer() << " is sending corrupted data";
          ++skipped;
          _rbuffer.pop(1);
        }
        else
          corrupted = false;
      }

      // Get compressed data.
      _get_data(size + sizeof(qint32), deadline);
      misc::shared_ptr<io::raw> r(new io::raw);

      // The requested data size might have not been read entirely
      // because of substream shutdown. This indicates that data is
      // corrupted because the size is greater than the remaining
      // payload size.
      if (_rbuffer.size() >= static_cast<int>(size + sizeof(qint32))) {
        r->QByteArray::operator=(_uncompress(
          static_cast<uchar const*>(static_cast<void const*>((
            _rbuffer.data() + sizeof(qint32)))),
          size));
      }
      if (!r->size()) { // No data or uncompressed size of 0 means corrupted input.
        logging::error(logging::low)
          << "compression: " << this
          << " got corrupted compressed data, skipping next byte";
        if (!skipped)
          logging::error(logging::high) << "compression: peer "
            << peer() << " is sending corrupted data";
        ++skipped;
        _rbuffer.pop(1);
        corrupted = true;
      }
      else {
        logging::debug(logging::low) << "compression: " << this
          << " uncompressed " << size + sizeof(qint32) << " bytes to "
          << r->size() << " bytes";
        data = r;
        _rbuffer.pop(size + sizeof(qint32));
        corrupted = false;
      }
    }
    if (skipped)
      logging::info(logging::high) << "compression: peer " << peer()
        << " sent " << skipped
        << " corrupted compressed bytes, resuming processing";
  }
  catch (exceptions::interrupt const& e) {
    (void)e;
    return (true);
  }
  catch (exceptions::timeout const& e) {
    (void)e;
    return (false);
  }
  catch (exceptions::shutdown const& e) {
    _shutdown = true;
    if (!_wbuffer.isEmpty()) {
      misc::shared_ptr<io::raw> r(new io::raw);
      *static_cast<QByteArray*>(r.data()) = _wbuffer;
      data = r;
      _wbuffer.clear();
    }
    else
      throw ;
  }

  return (true);
}

/**
 *  Get statistics.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(io::properties& tree) const {
  if (!_substream.isNull())
    _substream->statistics(tree);
  return ;
}

/**
 *  Flush the stream.
 *
 *  @return The number of events acknowledged.
 */
int stream::flush() {
  _flush();
  return (0);
}

/**
 *  @brief Write data.
 *
 *  The data can be buffered before being written to the subobject.
 *
 *  @param[in] d Data to send.
 *
 *  @return 1.
 */
int stream::write(misc::shared_ptr<io::data> const& d) {
  if (!validate(d, "compression"))
    return (1);

  // Check if substream is shutdown.
  if (_shutdown)
    throw (exceptions::shutdown() << "cannot write to compression "
           << "stream: sub-stream is already shutdown");

  // Process raw data only.
  if (d->type() == io::raw::static_type()) {
    io::raw const& r(d.ref_as<io::raw>());

    // Check length.
    if (r.size() > max_data_size)
      throw (exceptions::msg() << "cannot compress buffers longer than "
             << max_data_size << " bytes: you should report this error "
             << "to Centreon Broker developers");
    else if (r.size() > 0) {
      // Append data to write buffer.
      _wbuffer.append(r);

      // Send compressed data if size limit is reached.
      if (_wbuffer.size() >= _size)
        _flush();
    }
  }
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Flush data accumulated in write buffer.
 */
void stream::_flush() {
  // Check for shutdown stream.
  if (_shutdown)
    throw (exceptions::shutdown() << "cannot flush compression "
           << "stream: sub-stream is already shutdown");

  if (_wbuffer.size() > 0) {
    // Compress data.
    misc::shared_ptr<io::raw> compressed(new io::raw);
    compressed->QByteArray::operator=(_compress(_wbuffer, _level));
    logging::debug(logging::low) << "compression: " << this
      << " compressed " << _wbuffer.size() << " bytes to "
      << compressed->size() << " bytes (level " << _level << ")";
    _wbuffer.clear();

    // Add compressed data size.
    unsigned char buffer[4];
    unsigned int size(compressed->size());
    buffer[0] = size & 0xFF;
    buffer[1] = (size >> 8) & 0xFF;
    buffer[2] = (size >> 16) & 0xFF;
    buffer[3] = (size >> 24) & 0xFF;
    for (size_t i(0); i < sizeof(buffer); ++i)
      compressed->prepend(buffer[i]);

    // Send compressed data.
    _substream->write(compressed);
  }

  return ;
}

/**
 *  Get data with a size.
 *
 *  @param[in]  size       Data size to get.
 *  @param[in]  deadline   Timeout.
 */
void stream::_get_data(int size, time_t deadline) {
  try {
    while (_rbuffer.size() < size) {
      misc::shared_ptr<io::data> d;
      if (!_substream->read(d, deadline))
        throw (exceptions::timeout());
      else if (d.isNull())
        throw (exceptions::interrupt());
      else if (d->type() == io::raw::static_type()) {
        misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
        _rbuffer.push(*r);
      }
    }
  }
  // If the substream is shutdown, just indicates it and return already
  // read data. Caller will handle missing data.
  catch (exceptions::shutdown const& e) {
    (void)e;
    _shutdown = true;
  }
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void stream::_internal_copy(stream const& other) {
  _level = other._level;
  _rbuffer = other._rbuffer;
  _size = other._size;
  _wbuffer = other._wbuffer;
  return ;
}
