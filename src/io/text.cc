/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "io/text.hh"

using namespace io;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  text is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] t Unused.
 */
text::text(text const& t) : stream(t) {
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  text is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  @param[in] t Unused.
 *
 *  @return This object.
 */
text& text::operator=(text const& t) {
  (void)t;
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
 *  @param[in] s stream on which text should be based.
 */
text::text(stream* s)
  : _closed(false), _discard(0), _length(0), _stream(s) {
  _buffer[0] = '\0';
}

/**
 *  Destructor.
 */
text::~text() {}

/**
 *  Close the text stream.
 */
void text::close() {
  _stream->close();
  return ;
}

/**
 *  Extract the next available text line.
 *
 *  @return Next available line, NULL if no more line is available.
 */
char const* text::line() {
  // Discard previously read data.
  _length -= _discard;
  memmove(_buffer, _buffer + _discard, _length + 1);
  _discard = 0;

  // Read data as long as no carriage-return is encountered.
  size_t old_length(0);
  while (!strchr(_buffer + old_length, '\n')
         && _length < sizeof(_buffer) - 1
         && !_closed) {

      old_length = _length;
      unsigned int bytes_read(_stream->receive(_buffer + _length,
        sizeof(_buffer) - _length - 1));
      if (!bytes_read)
        _closed = true;
      else
        _length += bytes_read;
      _buffer[_length] = '\0'; // so that strchr does not fail
    }
  _discard = strcspn(_buffer, "\n");
  if (_buffer[_discard] != '\0')
    _buffer[_discard++] = '\0';
  return (_length ? _buffer : NULL);
}

/**
 *  Receive raw input from the stream.
 *
 *  @param[out] buffer Buffer that holds data.
 *  @param[in]  size   Maximum size to receive on buffer.
 *
 *  @return Number of bytes read from the stream.
 */
unsigned int text::receive(void* buffer, unsigned int size) {
  return (_stream->receive(buffer, size));
}

/**
 *  @brief Release the underlying stream.
 *
 *  Cause the underlying stream not to be destroyed upon Text destruction.
 */
void text::release() {
  _stream.release();
  return ;
}

/**
 *  Send raw data to the stream.
 *
 *  @param[in] buffer Data to send.
 *  @param[in] size   Maximum number of bytes to send.
 *
 *  @return Number of bytes actually sent.
 */
unsigned int text::send(void const* buffer, unsigned int size) {
  return (_stream->send(buffer, size));
}
