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

#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "exceptions/retval.hh"
#include "io/split.hh"

using namespace io;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Browse directory.
 *
 *  @param[out] min Minimum number found in directory.
 *  @param[out] max Maximum number found in directory.
 *
 *  @return Number of entries in the directory.
 */
unsigned int split::_browse_dir(unsigned int* min, unsigned int* max) {
  DIR* dir;
  dirent* entry;
  unsigned int entries;
  unsigned int my_min;
  unsigned int my_max;
  char* name;
  unsigned int namelen;
  char* path;

  // Open directory.
  path = new char[_basefile.size() + 1];
  strcpy(path, _basefile.c_str());
  dir = opendir(dirname(path));

  // Check if directory was successfully opened.
  if (!dir) {
    int e(errno);
    delete [] path;
    throw (exceptions::retval(e) << "could not open directory: "
                                 << strerror(e));
  }

  // Browse directory.
  entries = 0;
  my_min = UINT_MAX;
  my_max = 0;
  strcpy(path, _basefile.c_str());
  name = basename(path);
  namelen = strlen(name);
  while ((entry = readdir(dir))) {
    if (!strncmp(entry->d_name, name, namelen)) {
      char* ptr;
      unsigned int nb(strtoul(entry->d_name + namelen, &ptr, 10));
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
  if (min)
    *min = my_min;
  if (max)
    *max = my_max;
  return (entries);
}

/**
 *  @brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] s Object to copy.
 */
void split::_internal_copy(split const& s) {
  // Copy data members.
  _basefile = s._basefile;
  _current_in = s._current_in;
  _current_out = s._current_out;
  _max_file_size = s._max_file_size;
  _max_files = s._max_files;
  if (s._ofs.is_open()) {
    // Decrement current output file number so that OpenNextOutputFile()
    // properly open the current file.
    --_current_out;

    // Open next output file.
    if (_open_next_output_file())
      throw (exceptions::basic() << "could not open output file");
  }
  if (s._ifs.is_open()) {
    // Decrement current input file number so that OpenNextInputFile()
    // properly open the current file.
    --_current_in;
    if (_open_next_input_file())
      throw (exceptions::basic() << "could not open input file");
  }
  return ;
}

/**
 *  Open the next input file.
 *
 *  @return false if file was opened successfully.
 */
bool split::_open_next_input_file() {
  std::stringstream ss;

  // Close previously opened file.
  if (_ifs.is_open()) {
    if (_current_in < _current_out) {
      _ifs.close();
      ss << _basefile << _current_in;
      unlink(ss.str().c_str());
    }
    else{
      if ((_current_in == _current_out)
          && _ofs.is_open()) {
        _ofs.close();
        ss << _basefile << _current_in;
        unlink(ss.str().c_str());
      }
      return (true);
    }
  }

  // Find the lowest file id.
  _browse_dir(&_current_in);
  --_current_in;

  // Build file name.
  ss << _basefile << ++_current_in;

  // Open next file.
  _ifs.open(ss.str().c_str());

  // Return whether or not the file opening succeeded.
  return (!_ifs.is_open());
}

/**
 *  Open the next output file.
 */
bool split::_open_next_output_file() {
  unsigned int min;
  std::stringstream ss;

  // Close previously opened file.
  if (_ofs.is_open())
    _ofs.close();

  // Check that we are able to create a new file.
  if ((_max_files > 1) || !_ifs.is_open()) {
    // Check that number of entries in the directory is less than max_files.
    while (_browse_dir(&min) >= _max_files) {
      // Skip processed input file.
      if ((min == _current_in) && (_ifs.is_open()))
        continue ;

      // Remove file.
      ss << _basefile << min;
      if (unlink(ss.str().c_str())) {
        int e(errno);
        throw (exceptions::retval(e) << "could not remove file: "
                                     << strerror(errno));
      }
    }
  }
  else
    throw (exceptions::basic() << "could not create new split file");

  // Build file name.
  ss << _basefile << ++_current_out;

  // Open next file.
  _ofs.open(ss.str().c_str());
  _out_offset = 0;

  // Return whether or not the file opening succeeded.
  return (!_ofs.is_open());
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
split::split()
  : _current_in(0),
    _current_out(0),
    _max_file_size(UINT_MAX),
    _max_files(UINT_MAX),
    _out_offset(UINT_MAX) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
split::split(split const& s) : stream(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
split::~split() {
  close();
}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
split& split::operator=(split const& s) {
  _internal_copy(s);
  return (*this);
}

/**
 *  Set the base file name.
 *
 *  @param[in] basefile Base file name.
 */
void split::base_file(std::string const& basefile) {
  _basefile = basefile;
  return ;
}

/**
 *  Close all open file handles.
 */
void split::close() {
  close_input();
  close_output();
  return ;
}

/**
 *  Close input file handle.
 */
void split::close_input() {
  if (_ifs.is_open())
    _ifs.close();
  return ;
}

/**
 *  Close output file handle.
 */
void split::close_output() {
  if (_ofs.is_open())
    _ofs.close();
  return ;
}

/**
 *  Set the maximum size of one file.
 *
 *  @param[in] max_size Maximum size of one file.
 */
void split::max_file_size(unsigned int max_size) {
  _max_file_size = max_size;
  return ;
}

/**
 *  Set the maximum number of files that can be created.
 *
 *  @param[in] max_files Maximum number of files.
 */
void split::max_files(unsigned int max_files) {
  _max_files = max_files;
  return ;
}

/**
 *  Read data from split files.
 *
 *  @param[out] buffer Buffer where data will be stored.
 *  @param[in]  size   Maximum number of bytes to read.
 *
 *  @return Number of read bytes.
 */
unsigned int split::receive(void* buffer, unsigned int size) {
  // Run as long as not all data has been read.
  char* buf(static_cast<char*>(buffer));
  unsigned int remaining(size);
  while (remaining > 0) {
    unsigned int rb(_ifs.readsome(buf, remaining));
    if ((!rb || !_ifs.good()) && _open_next_input_file())
      break ;
    buf += rb;
    remaining -= rb;
  }

  // Return number of bytes actually received.
  return (size - remaining);
}

/**
 *  Write data to split files.
 *
 *  @param[in] buffer Buffer holding data to write.
 *  @param[in] size   Maximum number of bytes to write.
 *
 *  @return Number of written bytes.
 */
unsigned int split::send(void const* buffer, unsigned int size) {
  // Run as long as some data has not been sent.
  char const* buf(static_cast<const char*>(buffer));
  unsigned int remaining(size);
  while (remaining > 0) {
    unsigned int wb;

    // If we reached the end of the current file, go to the next one.
    if ((_out_offset >= _max_file_size) && _open_next_output_file())
      throw (exceptions::basic() << "could not open output file");

    // Compute how much bytes we can write.
    wb = _max_file_size - _out_offset;
    if (wb > remaining)
      wb = remaining;

    // Write data to the file.
    _ofs.write(buf, wb);
    buf += wb;
    _out_offset += wb;
    remaining -= wb;
  }

  // Return number of bytes actually written.
  return (size - remaining);
}
