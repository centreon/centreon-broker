/*
** Copyright 2011-2014 Centreon
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
  : _level(level), _process_in(true), _process_out(true), _size(size) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  _internal_copy(s);
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
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  io::stream::operator=(s);
  _internal_copy(s);
  return (*this);
}

/**
 *  Set which data to process.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void stream::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
  return ;
}

/**
 *  Read data.
 *
 *  @param[out] data Data packet.
 */
void stream::read(misc::shared_ptr<io::data>& data) {
  // Clear existing content.
  data.clear();

  // Check that data should be processed.
  if (!_process_in)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "compression stream is shutdown");

  try {
    // Compute compressed data length.
    if (_get_data(sizeof(qint32))) {
      int size;
      {
        unsigned char* buff((unsigned char*)_rbuffer.data());
        size = (buff[0] << 24)
               | (buff[1] << 16)
               | (buff[2] << 8)
               | (buff[3]);
      }

      // Get compressed data.
      if (_get_data(size + 4)) {
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

  return ;
}

/**
 *  Get statistics.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(io::properties& tree) const {
  if (!_to.isNull())
    _to->statistics(tree);
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
  // Check that data exists and should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "compression stream is shutdown");

  // Forced commit.
  if (d.isNull())
    _flush();
  else {
    // Process raw data only.
    if (d->type() == io::events::data_type<io::events::internal, 1>::value) {
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
    _to->write(compressed);
  }

  return ;
}

/**
 *  Get data with a fixed size.
 *
 *  @param[in] size Data size to get.
 *
 *  @return  True is enough data was found.
 */
bool stream::_get_data(unsigned int size) {
  bool retval = false;
  while (static_cast<unsigned int>(_rbuffer.size()) < size) {
    misc::shared_ptr<io::data> d;
    _from->read(d);
    if (d.isNull())
      break ;
    else {
      if (d->type() == io::events::data_type<io::events::internal, 1>::value) {
        misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
        _rbuffer.append(*r);
      }
    }
  }

  if (static_cast<unsigned int>(_rbuffer.size()) >= size)
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
  _process_in = s._process_in;
  _process_out = s._process_out;
  _rbuffer = s._rbuffer;
  _size = s._size;
  _wbuffer = s._wbuffer;
  return ;
}
