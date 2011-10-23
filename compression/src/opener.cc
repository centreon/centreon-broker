/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/compression/opener.hh"
#include "com/centreon/broker/compression/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
opener::opener() : io::endpoint(false), _level(-1), _size(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
opener::opener(opener const& o)
  : io::endpoint(o),
    _level(o._level),
    _size(o._size) {}

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
  if (this != &o) {
    io::endpoint::operator=(o);
    _level = o._level;
    _size = o._size;
  }
  return (*this);
}

/**
 *  Close the opener.
 */
void opener::close() {
  return ;
}

/**
 *  Open a compression stream.
 *
 *  @return New compression object.
 */
QSharedPointer<io::stream> opener::open() {
  return (QSharedPointer<io::stream>(new stream(_level, _size)));
}

/**
 *  Set the compression level.
 *
 *  @param[in] level Compression level (between 1 and 9, default is -1).
 */
void opener::set_level(int level) {
  _level = level;
  return ;
}

/**
 *  Set the compression buffer size.
 *
 *  @param[in] size Compression buffer size (default is 0 which means no
 *                  buffering).
 */
void opener::set_size(unsigned int size) {
  _size = size;
  return ;
}
