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
#include "file_output.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  FileOutput copy constructor.
 */
FileOutput::FileOutput(const FileOutput& fileo) : Thread()
{
  (void)fileo;
}

/**
 *  FileOutput operator= overload.
 */
FileOutput& FileOutput::operator=(const FileOutput& fileo)
{
  (void)fileo;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  FileOutput default constructor.
 */
FileOutput::FileOutput()
{
  this->buffer.clear();
}

/**
 *  FileOutput destructor.
 */
FileOutput::~FileOutput()
{
}

/**
 *  Closes the file.
 */
void FileOutput::Close()
{
  assert(this->ofs.is_open());
  this->exit_thread = true;
  this->condvar.Broadcast();
  this->Join();
  this->ofs.close();
  return ;
}

/**
 *  Opens the file.
 */
void FileOutput::Open(const std::string& filename) throw (Exception)
{
  // XXX : open might throw exceptions
  this->ofs.open(filename.c_str());
  if (this->ofs.fail())
    throw (Exception("Could not open output file."));
  this->exit_thread = false;
  // XXX : catch Run() exceptions (or not).
  this->Run();
  return ;
}

/**
 *  Thread core function.
 */
int FileOutput::Core()
{
  char buff[4096];
  size_t wb;

  while (!this->exit_thread || !this->buffer.empty())
    {
      // XXX : catch exceptions
      this->mutex.Lock();
      if (this->buffer.empty())
	this->condvar.Wait(this->mutex);
      wb = this->buffer.copy(buff, sizeof(buff));
      this->buffer.erase(0, wb);
      this->mutex.Unlock();
      this->ofs.write(buff, wb);
    }
  return (0);
}
