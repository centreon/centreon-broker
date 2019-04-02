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

#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/compression/zlib.hh"
#include "com/centreon/broker/exceptions/corruption.hh"
#include "com/centreon/broker/exceptions/interrupt.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/exceptions/timeout.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

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
               std::shared_ptr<io::data>& data,
               time_t deadline) {
  // Clear existing content.
  data.reset();

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
      std::shared_ptr<io::raw> r(new io::raw);

      // The requested data size might have not been read entirely
      // because of substream shutdown. This indicates that data is
      // corrupted because the size is greater than the remaining
      // payload size.
      if (_rbuffer.size() >= static_cast<int>(size + sizeof(qint32))) {
        try {
          r->QByteArray::operator=(zlib::uncompress(
            static_cast<unsigned char const*>(static_cast<void const*>((
              _rbuffer.data() + sizeof(qint32)))),
            size));
        }
        catch (exceptions::corruption const& e) {
          logging::debug(logging::medium) << e.what();
        }
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
      std::shared_ptr<io::raw> r(new io::raw);
      *static_cast<QByteArray*>(r.get()) = _wbuffer;
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
  if (_substream)
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
int stream::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, "compression"))
    return (1);

  // Check if substream is shutdown.
  if (_shutdown)
    throw (exceptions::shutdown() << "cannot write to compression "
           << "stream: sub-stream is already shutdown");

  // Process raw data only.
  if (d->type() == io::raw::static_type()) {
    io::raw const& r(*std::static_pointer_cast<io::raw>(d));

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
    std::shared_ptr<io::raw> compressed(new io::raw);
    compressed->QByteArray::operator=(zlib::compress(_wbuffer, _level));
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
      std::shared_ptr<io::data> d;
      if (!_substream->read(d, deadline))
        throw (exceptions::timeout());
      else if (!d)
        throw (exceptions::interrupt());
      else if (d->type() == io::raw::static_type()) {
        std::shared_ptr<io::raw> r(std::static_pointer_cast<io::raw>(d));
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
