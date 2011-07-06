/*
** Copyright 2011 Merethis
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

#include <QSharedPointer>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Get data with a fixed size.
 *
 *  @param[in] size Data size to get.
 */
bool stream::_get_data(unsigned int size) {
  bool retval;
  if (static_cast<unsigned int>(_rbuffer.size()) < size) {
    QSharedPointer<io::data> d(_from->read());
    if (!d.isNull())
      retval = false;
    else {
      if (d->type() == "com::centreon::broker::io::data") {
        QSharedPointer<io::raw> r(d.staticCast<io::raw>());
        _rbuffer.append(*r);
      }
      retval = _get_data(size);
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
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
stream::~stream() {}

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
 *  Read data.
 *
 *  @return Data packet.
 */
QSharedPointer<io::data> stream::read() {
  // Return value.
  QSharedPointer<io::data> data;

  // Compute compressed data length.
  if (_get_data(sizeof(qint32))) {
    int size;
    {
      char* buff(_rbuffer.data());
      size = (buff[0] << 24)
             | (buff[1] << 16)
             | (buff[2] << 8)
             | (buff[3]);
    }

    // Get compressed data.
    if (_get_data(size + 4)) {
      QSharedPointer<io::raw> r(new io::raw);
      r->QByteArray::operator=(qUncompress(static_cast<uchar*>(
                                 static_cast<void*>((_rbuffer.data()
                                   + 4))),
                               size));
      data = r.staticCast<io::data>();
      _rbuffer.remove(0, size + 4);
    }
    else
      _rbuffer.clear();
  }
  else
    _rbuffer.clear();
  return (data);
}

/**
 *  @brief Write data.
 *
 *  The data can be buffered before being written to the subobject.
 *
 *  @param[in] d Data to send.
 */
void stream::write(QSharedPointer<io::data> d) {
  // Process raw data only.
  if (d->type() == "com::centreon::broker::io::raw") {
    // Append data to write buffer.
    QSharedPointer<io::raw> r(d.staticCast<io::raw>());
    _wbuffer.append(*r);

    // Send compressed data if size limit is reached.
    if (static_cast<unsigned int>(_wbuffer.size()) >= _size) {
      QSharedPointer<io::raw> compressed(new io::raw);
      compressed->QByteArray::operator=(qCompress(_wbuffer, _level));
      _wbuffer.clear();
      _to->write(compressed);
    }
  }
  return ;
}
