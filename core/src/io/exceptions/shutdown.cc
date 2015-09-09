/*
** Copyright 2011 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
