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

#include <cassert>
#include "exception.h"
#include "protocol_socket.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  ProtocolSocket copy constructor.
 */
ProtocolSocket::ProtocolSocket(const ProtocolSocket& ps) throw ()
{
  (void)ps;
  assert(false);
}

/**
 *  ProtocolSocket operator= overload.
 */
ProtocolSocket& ProtocolSocket::operator=(const ProtocolSocket& ps) throw ()
{
  (void)ps;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  ProtocolSocket constructor.
 */
ProtocolSocket::ProtocolSocket(IO::Stream* stream) throw () : stream_(stream)
{
  this->buffer_[0] = '\0';
  this->bytes_processed_ = 0L;
  this->discard_ = 0;
  this->last_checkin_time_ = time(NULL);
  this->length_ = 0;
  this->lines_processed_ = 0L;
}

/**
 *  ProtocolSocket destructor.
 */
ProtocolSocket::~ProtocolSocket()
{
  this->stream_->Close();
  delete (this->stream_);
}

/**
 *  Returns the number of bytes processed by the socket.
 */
unsigned long ProtocolSocket::GetBytesProcessed() const
{
  return (this->bytes_processed_);
}

/**
 *  Returns the last time the client had an activity.
 */
time_t ProtocolSocket::GetLastCheckinTime() const
{
  return (this->last_checkin_time_);
}

char* ProtocolSocket::GetLine()
{
  int old_length;

  this->length_ -= this->discard_;
  memmove(this->buffer_,
	  this->buffer_ + this->discard_,
	  this->length_ + 1);
  this->discard_ = 0;
  old_length = 0;
  while (!strchr(this->buffer_ + old_length, '\n')
	 && this->length_ < sizeof(this->buffer_) - 1)
    {
      unsigned long bytes_read;

      old_length = this->length_;
      bytes_read = this->stream_->Receive(this->buffer_ + this->length_,
                     sizeof(this->buffer_) - this->length_ - 1);
      this->bytes_processed_ += bytes_read;
      this->length_ += bytes_read;
      this->buffer_[this->length_] = '\0';
      this->last_checkin_time_ = time(NULL);
      // XXX : find a better way to correct this
      if (!bytes_read)
	throw (Exception(0, "Socket is closed"));
    }
  this->discard_ = strcspn(this->buffer_, "\n");
  this->buffer_[this->discard_++] = '\0';
  ++this->lines_processed_;
  return (this->buffer_);
}

/**
 *  Returns the number of lines processed by the socket.
 */
unsigned long ProtocolSocket::GetLinesProcessed() const
{
  return (this->lines_processed_);
}
