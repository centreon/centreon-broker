/*
** Copyright 2009-2011 MERETHIS
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

#include <assert.h>
#include <stdlib.h>
#include "interface/xml/source.hh"

using namespace interface::xml;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters from the data stream.
 */
template <typename T>
static T* handle_event() {
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  source is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] s Unused.
 */
source::source(source const& s) : interface::source(s) {
  (void)s;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  source is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  @param[in] s Unused.
 *
 *  @return This object.
 */
source& source::operator=(source const& s) {
  (void)s;
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
 *  Constructor.
 *
 *  @param[in] s Stream on which data will be sent.
 */
source::source(io::stream* s) : _stream(s) {}

/**
 *  Destructor.
 */
source::~source() {}

/**
 *  Close the underlying stream.
 */
void source::close() {
  _stream->close();
  return ;
}

/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @return Next available event, NULL if stream is closed.
 */
events::event* source::event() {
}
