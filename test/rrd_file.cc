/*
** Copyright 2012 Merethis
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

#include <cstdlib>
#include <cstring>
#include <rrd.h>
#include "test/rrd_file.hh"

/**************************************
*                                     *
*           Local Functions           *
*                                     *
**************************************/

/**
 *  Helper callback for RRD file dump.
 *
 *  @param[in] data RRD data.
 *  @param[in] size Data size.
 *  @param[in] ptr  rrd_file pointer.
 */
static size_t dump_callback(void const* data, size_t size, void* ptr) {
  static_cast<rrd_file*>(ptr)->add_data(data, size);
  return (size);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
rrd_file::rrd_file() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
rrd_file::rrd_file(rrd_file const& right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
rrd_file::~rrd_file() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
rrd_file& rrd_file::operator=(rrd_file const& right) {
  if (this != &right)
    _internal_copy(right);
  return (*this);
}

/**
 *  Add data of RRD file.
 *
 *  @param[in] data Data.
 *  @param[in] size Size of data.
 */
void rrd_file::add_data(void const* data, size_t size) {
  _buffer.append(static_cast<char const*>(data), size);
  size_t pos(_buffer.find_first_of('\n'));
  if (pos != std::string::npos) {
    _buffer[pos] = '\0';
    _process_line(_buffer.c_str());
    _buffer.erase(0, pos + 1);
  }
  return ;
}

/**
 *  Get the RRAs.
 *
 *  @return Loaded RRAs.
 */
std::list<std::map<time_t, double> > const& rrd_file::get_rras() const throw () {
  return (_rras);
}

/**
 *  Load some RRD file.
 *
 *  @param[in] file Path to the RRD file.
 */
void rrd_file::load(char const* file) {
  _in_rra = false;
  rrd_dump_cb_r(file, 0, &dump_callback, this);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void rrd_file::_internal_copy(rrd_file const& right) {
  _rras = right._rras;
  return ;
}

/**
 *  Process a RRD dump line.
 *
 *  @param[in] line RRD dump line.
 */
void rrd_file::_process_line(char const* line) {
  line += strspn(line, " \t");
  if (!strcmp(line, "<rra>")) {
    _rras.push_back(std::map<time_t, double>());
    _in_rra = true;
  }
  else if (!strcmp(line, "</rra>"))
    _in_rra = false;
  else if (_in_rra && !strncmp(line, "<!-- ", 5)) {
    line = strchr(line, '/') + 2;
    char* ptr;
    time_t t(strtoul(line, &ptr, 10));
    if (*ptr == ' ')
      ++ptr;
    if (*ptr == '-') {
      line = ptr;
      line += strlen("--> <row><v>");
      double v(strtod(line, NULL));
      _rras.back()[t] = v;
    }
  }
  return ;
}
