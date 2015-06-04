/*
** Copyright 2011-2015 Merethis
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

#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/exceptions/timeout.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
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
stream::stream(int level, unsigned int size)
  : _level(level), _size(size) {}

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
  _flush();
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
    // Compute compressed data length.
    if (_get_data(sizeof(qint32), deadline)) {
      int size;
      {
        unsigned char* buff((unsigned char*)_rbuffer.data());
        size = (buff[0] << 24)
               | (buff[1] << 16)
               | (buff[2] << 8)
               | (buff[3]);
      }

      // Get compressed data.
      if (_get_data(size + 4, deadline)) {
        misc::shared_ptr<io::raw> r(new io::raw);
        r->QByteArray::operator=(qUncompress(static_cast<uchar*>(
                                   static_cast<void*>((_rbuffer.data()
                                                       + 4))),
                                   size));
        logging::debug(logging::low) << "compression: " << this
          << " uncompressed " << size + 4 << " bytes to " << r->size()
          << " bytes";
        data = r;
        _rbuffer.remove(0, size + 4);
      }
      else
        _rbuffer.clear();
    }
    else
      _rbuffer.clear();
  }
  catch (exceptions::timeout const& e) {
    (void)e;
    return (false);
  }
  catch (io::exceptions::shutdown const& e) {
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
 *  @brief Write data.
 *
 *  The data can be buffered before being written to the subobject.
 *
 *  @param[in] d Data to send.
 *
 *  @return Number of events acknowledged (1).
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  // Forced commit.
  if (d.isNull())
    _flush();
  else {
    // Process raw data only.
    if (d->type() == io::raw::static_type()) {
      // Append data to write buffer.
      misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
      _wbuffer.append(*r);

      // Send compressed data if size limit is reached.
      if (static_cast<unsigned int>(_wbuffer.size()) >= _size)
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
  if (_wbuffer.size() > 0) {
    // Compress data.
    misc::shared_ptr<io::raw> compressed(new io::raw);
    compressed->QByteArray::operator=(qCompress(_wbuffer, _level));
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
 *  Get data with a fixed size.
 *
 *  @param[in]  size       Data size to get.
 *  @param[in]  deadline   Timeout.
 */
bool stream::_get_data(
               unsigned int size,
               time_t deadline) {
  bool retval;
  if (static_cast<unsigned int>(_rbuffer.size()) < size) {
    misc::shared_ptr<io::data> d;
    if (!_substream->read(d, deadline))
      throw (exceptions::timeout());
    if (d.isNull())
      retval = false;
    else {
      if (d->type() == io::raw::static_type()) {
        misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
        _rbuffer.append(*r);
      }
      retval = _get_data(size, deadline);
    }
  }
  else
    retval = true;
  return (retval);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] s Object to copy.
 */
void stream::_internal_copy(stream const& s) {
  _level = s._level;
  _rbuffer = s._rbuffer;
  _size = s._size;
  _wbuffer = s._wbuffer;
  return ;
}
