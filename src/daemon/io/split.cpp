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

#include <dirent.h>    // for opendir, readdir
#include <errno.h>     // for errno
#include <libgen.h>    // for basename, dirname
#include <limits.h>
#include <sstream>
#include <stdlib.h>    // for strtoul
#include <string.h>    // for strerror
#include <unistd.h>    // for unlink
#include "exception.h"
#include "io/split.h"

using namespace IO;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Browse directory.
 *
 *  \param[out] min Minimum number found in directory.
 *  \param[out] max Maximum number found in directory.
 *
 *  \return Number of entries in the directory.
 */
unsigned int Split::BrowseDir(unsigned int* min, unsigned int* max)
{
  DIR* dir;
  dirent* entry;
  unsigned int entries;
  unsigned int my_min;
  unsigned int my_max;
  char* name;
  unsigned int namelen;
  char* path;

  // Open directory.
  path = new char[this->basefile_.size() + 1];
  strcpy(path, this->basefile_.c_str());
  dir = opendir(dirname(path));

  // Check if directory was successfully opened.
  if (!dir)
    {
      delete [] path;
      throw (Exception(errno, strerror(errno)));
    }

  // Browse directory.
  entries = 0;
  my_min = UINT_MAX;
  my_max = 0;
  strcpy(path, this->basefile_.c_str());
  name = basename(path);
  namelen = strlen(name);
  while ((entry = readdir(dir)))
    {
      if (!strncmp(entry->d_name, name, namelen))
	{
          char* ptr;
	  unsigned int nb;

	  nb = strtoul(entry->d_name + namelen, &ptr, 10);
          if (*ptr) // Last part of filename wasn't a proper number.
            continue ;
	  if (my_min > nb)
	    my_min = nb;
	  if (my_max < nb)
	    my_max = nb;
	}
      ++entries;
    }
  closedir(dir);

  // Free memory.
  delete [] path;

  // Return found values.
  *min = my_min;
  *max = my_max;
  return (entries);
}

/**
 *  \brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] split Object to copy.
 */
void Split::InternalCopy(const Split& split)
{
  // Copy data members.
  this->basefile_ = split.basefile_;
  this->current_in_ = split.current_in_;
  this->current_out_ = split.current_out_;
  this->max_file_size_ = split.max_file_size_;
  this->max_files_ = split.max_files_;

  if (split.ofs_.is_open())
    {
      // Decrement current output file number so that OpenNextOutputFile()
      // properly open the current file.
      --this->current_out_;

      // Open next output file.
      if (this->OpenNextOutputFile())
        throw (Exception(0, "Could not open output file."));
    }
  if (split.ifs_.is_open())
    {
      // Decrement current input file number so that OpenNextInputFile()
      // properly open the current file.
      --this->current_in_;
      if (this->OpenNextInputFile())
        throw (Exception(0, "Could not open input file."));
    }
  return ;
}

/**
 *  Open the next input file.
 *
 *  \return false if file was opened successfully.
 */
bool Split::OpenNextInputFile()
{
  std::stringstream ss;

  // Close previously opened file.
  if (this->ifs_.is_open())
    this->ifs_.close();

  // Build file name.
  ss << this->basefile_ << ++this->current_in_;

  // Open next file.
  this->ifs_.open(ss.str().c_str());

  // Return whether or not the file opening succeeded.
  return (this->ifs_);
}

/**
 *  Open the next output file.
 */
bool Split::OpenNextOutputFile()
{
  unsigned int min;
  std::stringstream ss;

  // Close previously opened file.
  if (this->ofs_.is_open())
    this->ofs_.close();

  // Check that we are able to create a new file.
  if ((this->max_files_ > 1) || !this->ifs_.is_open())
    {
      // Check that number of entries in the directory is less than max_files.
      while (this->BrowseDir(&min) >= this->max_files_)
        {
          // Skip processed input file.
          if ((min == this->current_in_) && (this->ifs_.is_open()))
            continue ;

          // Remove file.
          ss << this->basefile_ << min;
          if (unlink(ss.str().c_str()))
            throw (Exception(errno, strerror(errno)));
        }
    }
  else
    throw (Exception(0, "Could not create new split file."));

  // Build file name.
  ss << this->basefile_ << ++this->current_out_;

  // Open next file.
  this->ofs_.open(ss.str().c_str());

  // Return whether or not the file opening succeeded.
  return (this->ofs_);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Split default constructor.
 */
Split::Split()
  : current_in_(0),
    current_out_(0),
    max_file_size_(UINT_MAX),
    max_files_(UINT_MAX),
    out_offset_(0) {}

/**
 *  Split copy constructor.
 *
 *  \param[in] split Split object to copy.
 */
Split::Split(const Split& split) : Stream(split)
{
  this->InternalCopy(split);
}

/**
 *  Split destructor.
 */
Split::~Split()
{
  this->Close();
}

/**
 *  Assignment operator overload.
 *
 *  \param[in] split Split object to copy.
 *
 *  \return *this
 */
Split& Split::operator=(const Split& split)
{
  this->InternalCopy(split);
  return (*this);
}

/**
 *  Set the base file name.
 *
 *  \param[in] basefile Base file name.
 */
void Split::BaseFile(const std::string& basefile)
{
  this->basefile_ = basefile;
  return ;
}

/**
 *  Close all open file handles.
 */
void Split::Close()
{
  this->CloseInput();
  this->CloseOutput();
  return ;
}

/**
 *  Close input file handle.
 */
void Split::CloseInput()
{
  if (this->ifs_.is_open())
    this->ifs_.close();
  return ;
}

/**
 *  Close output file handle.
 */
void Split::CloseOutput()
{
  if (this->ofs_.is_open())
    this->ofs_.close();
  return ;
}

/**
 *  Set the maximum size of one file.
 *
 *  \param[in] max_size Maximum size of one file.
 */
void Split::MaxFileSize(unsigned int max_size)
{
  this->max_file_size_ = max_size;
  return ;
}

/**
 *  Set the maximum number of files that can be created.
 *
 *  \param[in] max_files Maximum number of files.
 */
void Split::MaxFiles(unsigned int max_files)
{
  this->max_files_ = max_files;
  return ;
}

/**
 *  Read data from split files.
 *
 *  \param[out] buffer Buffer where data will be stored.
 *  \param[in]  size   Maximum number of bytes to read.
 *
 *  \return Number of read bytes.
 */
unsigned int Split::Receive(void* buffer, unsigned int size)
{
  char *buf;
  unsigned int remaining;

  // Check if file has been opened.
  if (!this->ifs_.is_open())
    if (this->OpenNextInputFile())
      throw (Exception(0, "Could not open input file."));

  // Run as long as not all data has been read.
  buf = static_cast<char*>(buffer);
  remaining = size;
  while (remaining > 0)
    {
      unsigned int rb;

      rb = this->ifs_.readsome(buf, size);
      if (!rb)
        {
          this->CloseInput();
          if (this->OpenNextInputFile())
            break ;
        }
      buf += rb;
      remaining -= rb;
    }

  // Return number of bytes actually received.
  return (size - remaining);
}

/**
 *  Write data to split files.
 *
 *  \param[in] buffer Buffer holding data to write.
 *  \param[in] size   Maximum number of bytes to write.
 *
 *  \return Number of written bytes.
 */
unsigned int Split::Send(const void* buffer, unsigned int size)
{
  const char* buf;
  unsigned int remaining;

  // Run as long as some data has not been sent.
  buf = static_cast<const char*>(buffer);
  remaining = size;
  while (remaining > 0)
    {
      unsigned int wb;

      // If we reached the end of the current file, go to the next one.
      if (this->out_offset_ >= this->max_file_size_)
        if (this->OpenNextOutputFile())
          throw (Exception(0, "Could not open output file."));

      // Compute how much bytes we can write.
      wb = this->max_file_size_ - this->out_offset_;
      if (wb > remaining)
        wb = remaining;

      // Write data to the file.
      this->ofs_.write(buf, wb);
      buf += wb;
      this->out_offset_ += wb;
      remaining -= wb;
    }

  // Return number of bytes actually written.
  return (size - remaining);
}
