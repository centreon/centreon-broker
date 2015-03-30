/*
** Copyright 2012,2015 Merethis
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

#include "com/centreon/broker/command_file/endpoint.hh"
#include "com/centreon/broker/command_file/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::command_file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
endpoint::endpoint(std::string const& filename)
  : io::endpoint(false),
    _filename(filename) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
endpoint::endpoint(endpoint const& o)
  : io::endpoint(o),
    _filename(o._filename) {}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
endpoint& endpoint::operator=(endpoint const& o) {
  io::endpoint::operator=(o);
  _filename = o._filename;
  return (*this);
}

/**
 *  Clone the opener.
 *
 *  @return This object.
 */
io::endpoint* endpoint::clone() const {
  return (new endpoint(*this));
}

/**
 *  Close the opener.
 */
void endpoint::close() {
  return ;
}

/**
 *  Open a new stream.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> endpoint::open() {
  return (misc::make_shared(new stream(_filename)));
}

/**
 *  Open a new stream.
 *
 *  @param[in] id     The identifier.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> endpoint::open(QString const& id) {
  return (misc::make_shared(new stream(_filename)));
}
