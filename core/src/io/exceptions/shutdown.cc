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

#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker::io::exceptions;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] s Object to copy.
 */
void shutdown::_internal_copy(shutdown const& s) {
  _in_shutdown = s._in_shutdown;
  _out_shutdown = s._out_shutdown;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] in_shutdown  Set to true if stream's input is shutdown.
 *  @param[in] out_shutdown Set to true if stream's output is shutdown.
 */
shutdown::shutdown(bool in_shutdown, bool out_shutdown)
  : _in_shutdown(in_shutdown), _out_shutdown(out_shutdown) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
shutdown::shutdown(shutdown const& s)
  : com::centreon::broker::exceptions::msg(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
shutdown::~shutdown() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
shutdown& shutdown::operator=(shutdown const& s) {
  if (this != &s) {
    com::centreon::broker::exceptions::msg::operator=(s);
    _internal_copy(s);
  }
  return (*this);
}

/**
 *  Clone this exception.
 *
 *  @return Newly allocated copy of this exception.
 */
com::centreon::broker::exceptions::msg* shutdown::clone() const {
  return (new shutdown(*this));
}

/**
 *  Check whether source stream has its input shutdown.
 *
 *  @return true if stream's input is shutdown.
 */
bool shutdown::is_in_shutdown() const {
  return (_in_shutdown);
}

/**
 *  Check whether source stream has its output shutdown.
 *
 *  @return true if stream's output is shutdown.
 */
bool shutdown::is_out_shutdown() const {
  return (_out_shutdown);
}

/**
 *  Rethrow this exception.
 */
void shutdown::rethrow() const {
  throw (*this);
  return ;
}
