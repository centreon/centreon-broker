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

#include <arpa/inet.h>
#include <cstring>
#include <sstream>
#include <stdint.h>
#include <string>
#include "dbuffer.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  DBuffer default constructor.
 */
DBuffer::DBuffer() {}

/**
 *  \brief DBuffer copy constructor.
 *
 *  Build the new dynamic buffer by copying data from the given object to the
 *  current instance.
 *
 *  \param[in] dbuffer Object to copy data from.
 */
DBuffer::DBuffer(const DBuffer& dbuffer)
{
  this->buffer_ = dbuffer.buffer_;
}

/**
 *  \brief DBuffer destructor.
 *
 *  Release ressources allocated by the dynamic buffer.
 */
DBuffer::~DBuffer() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] dbuffer Object to copy data from.
 *
 *  \return *this
 */
DBuffer& DBuffer::operator=(const DBuffer& dbuffer)
{
  this->buffer_ = dbuffer.buffer_;
  return (*this);
}

/**
 *  \brief Convert a dynamic buffer to a const void*.
 *
 *  Provides a dynamic buffer data as a C-style array.
 */
DBuffer::operator const void*() const
{
  return (this->buffer_.c_str());
}

/**
 *  Append a boolean to the dynamic buffer.
 *
 *  \param[in] b Value to append.
 */
void DBuffer::Append(bool b)
{
  uint8_t u8;

  u8 = b;
  this->buffer_.append((const char*)&u8, sizeof(u8));
  return ;
}

/**
 *  Append a C string to the dynamic buffer.
 *
 *  \param[in] s String to append.
 */
void DBuffer::Append(const char* s)
{
  size_t size;

  size = strlen(s);
  this->Append((short)size);
  this->buffer_.append(s, size);
  return ;
}

/**
 *  Append a double to the dynamic buffer.
 *
 *  \param[in] d Value to append.
 */
void DBuffer::Append(double d)
{
  std::ostringstream ss;

  ss << d;
  this->Append(ss.str());
  return ;
}

/**
 *  Append an integer to the dynamic buffer.
 *
 *  \param[in] i Value to append.
 */
void DBuffer::Append(int i)
{
  uint32_t u32;

  u32 = htonl(i);
  this->buffer_.append((const char*)&u32, sizeof(u32));
  return ;
}

/**
 *  Append a short to the dynamic buffer.
 *
 *  \param[in] s Value to append.
 */
void DBuffer::Append(short s)
{
  uint16_t u16;

  u16 = htons(s);
  this->buffer_.append((const char*)&u16, sizeof(u16));
  return ;
}

/**
 *  Append a string to the dynamic buffer.
 *
 *  \param[in] s String to append.
 */
void DBuffer::Append(const std::string& s)
{
  this->Append((short)s.size());
  this->buffer_.append(s);
  return ;
}

/**
 *  Get the size of dynamic buffer's data in bytes.
 *
 *  \return The size of the dynamic buffer's data in bytes.
 */
unsigned int DBuffer::Size() const
{
  return (this->buffer_.size());
}
