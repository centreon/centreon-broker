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
  : _is_in(is_in), _is_out(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
opener::opener(opener const& o)
  : io::endpoint(o),
    _filename(o._filename),
    _is_in(o._is_in),
    _is_out(o._is_out) {}

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
  return (*this);
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
QSharedPointer<io::stream> opener::open() {
  QIODevice::OpenMode mode(QIODevice::Append);
  if (_is_in)
    mode |= (_is_out ? QIODevice::ReadWrite : QIODevice::ReadOnly);
  else if (_is_out)
    mode |= QIODevice::WriteOnly;
  else
    throw (exceptions::msg() << "file: attempt to open a file with invalid flags");
  return (QSharedPointer<io::stream>(new stream(_filename, mode)));
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
