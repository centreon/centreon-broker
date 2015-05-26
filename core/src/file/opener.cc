/*
** Copyright 2011-2012,2015 Merethis
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

#include <QString>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/opener.hh"
#include "com/centreon/broker/file/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
opener::opener(bool is_in, bool is_out)
  : endpoint(false),
    _is_in(is_in),
    _is_out(is_out),
    _max_size(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
opener::opener(opener const& o)
  : io::endpoint(o),
    _filename(o._filename),
    _is_in(o._is_in),
    _is_out(o._is_out),
    _max_size(o._max_size) {}

/**
 *  Destructor.
 */
opener::~opener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
opener& opener::operator=(opener const& o) {
  io::endpoint::operator=(o);
  _filename = o._filename;
  _is_in = o._is_in;
  _is_out = o._is_out;
  _max_size = o._max_size;
  return (*this);
}

/**
 *  Clone the opener.
 *
 *  @return This object.
 */
io::endpoint* opener::clone() const {
  return (new opener(*this));
}

/**
 *  Close the opener.
 */
void opener::close() {
  return ;
}

/**
 *  Open a new stream.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> opener::open() {
  QString filename(_filename);
  return (misc::shared_ptr<io::stream>(
            new stream(qPrintable(filename), _max_size)));
}

/**
 *  Set the filename.
 *
 *  @param[in] filename Filename.
 */
void opener::set_filename(QString const& filename) {
  _filename = filename;
  return ;
}

/**
 *  Set the maximum allowed file size.
 *
 *  @param[in] max Maximum file size in bytes.
 */
void opener::set_max_size(unsigned long long max) {
  _max_size = max;
  return ;
}
