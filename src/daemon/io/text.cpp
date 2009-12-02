/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <stdlib.h>  // for abort
#include <string.h>
#include "io/text.h"

using namespace IO;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Text copy constructor.
 *
 *  Text is not copyable. Therefore any attempt to use the copy constructor
 *  will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] text Unused.
 */
Text::Text(const Text& text) : Stream(text)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Text is not copyable. Therefore any attempt to use the assignment operator
 *  will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] text Unused.
 *
 *  \return *this
 */
Text& Text::operator=(const Text& text)
{
  (void)text;
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
 *  Text constructor.
 *
 *  \param[in] stream Stream on which Text should be based.
 */
Text::Text(Stream* stream)
  : closed_(false), discard_(0), length_(0), stream_(stream) {}

/**
 *  Text destructor.
 */
Text::~Text() {}

/**
 *  Extract the next available text line.
 *
 *  \return Next available line, NULL if no more line is available.
 */
const char* Text::Line()
{
  size_t old_length;

  // Discard previously read data.
  this->length_ -= this->discard_;
  memmove(this->buffer_, this->buffer_ + this->discard_, this->length_ + 1);
  this->discard_ = 0;

  // Read data as long as no carriage-return is encountered.
  old_length = 0;
  while (!strchr(this->buffer_ + old_length, '\n')
         && this->length_ < sizeof(this->buffer_) - 1
         && !this->closed_)
    {
      unsigned int bytes_read;

      old_length = this->length_;
      bytes_read = this->stream_->Receive(this->buffer_ + this->length_,
                     sizeof(this->buffer_) - this->length_ - 1);
      if (!bytes_read)
        this->closed_ = true;
      this->length_ += bytes_read;
      this->buffer_[this->length_] = '\0'; // so that strchr does not fail
    }
  this->discard_ = strcspn(this->buffer_, "\n");
  this->buffer_[this->discard_++] = '\0';
  return (this->buffer_[0] ? this->buffer_ : NULL);
}

/**
 *  Receive raw input from the stream.
 *
 *  \param[out] buffer Buffer that holds data.
 *  \param[in]  size   Maximum size to receive on buffer.
 *
 *  \return Number of bytes read from the stream.
 */
unsigned int Text::Receive(void* buffer, unsigned int size)
{
  return (this->stream_->Receive(buffer, size));
}

/**
 *  \brief Release the underlying stream.
 *
 *  Cause the underlying stream not to be destroyed upon Text destruction.
 *  \par Safety No throw guarantee.
 */
void Text::Release()
{
  this->stream_.release();
  return ;
}

/**
 *  Send raw data to the stream.
 *
 *  \param[in] buffer Data to send.
 *  \param[in] size   Maximum number of bytes to send.
 *
 *  \return Number of bytes actually sent.
 */
unsigned int Text::Send(const void* buffer, unsigned int size)
{
  return (this->stream_->Send(buffer, size));
}
