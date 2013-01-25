/*
** Copyright 2013 Merethis
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

#include <memory>
#include "com/centreon/broker/bbdo/connector.hh"
#include "com/centreon/broker/bbdo/input.hh"
#include "com/centreon/broker/bbdo/output.hh"
#include "com/centreon/broker/bbdo/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] is_in  Connector should act as input.
 *  @param[in] is_out Connector should act as output.
 */
connector::connector(bool is_in, bool is_out)
  : io::endpoint(false), _is_in(is_in), _is_out(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
connector::connector(connector const& right)
  : io::endpoint(right), _is_in(right._is_in), _is_out(right._is_out) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& right) {
  if (this != &right) {
    io::endpoint::operator=(right);
    _is_in = right._is_in;
    _is_out = right._is_out;
  }
  return (*this);
}

/**
 *  Clone the connector.
 *
 *  @return A copy of this object.
 */
io::endpoint* connector::clone() const {
  std::auto_ptr<connector> copy(new connector(*this));
  if (!_from.isNull())
    copy->_from = _from->clone();
  return (copy.release());
}

/**
 *  Close the connector.
 */
void connector::close() {
  return ;
}

/**
 *  Open the connector.
 *
 *  @return Open stream.
 */
misc::shared_ptr<io::stream> connector::open() {
  misc::shared_ptr<io::stream> retval;
  if (!_from.isNull()) {
    retval = _from->open();
    misc::shared_ptr<io::stream> bbdo_stream;
    if (!retval.isNull()) {
      if (_is_in) {
        if (_is_out)
          bbdo_stream = misc::shared_ptr<io::stream>(new bbdo::stream);
        else
          bbdo_stream = misc::shared_ptr<io::stream>(new bbdo::input);
      }
      else
        bbdo_stream = misc::shared_ptr<io::stream>(new bbdo::output);
      bbdo_stream->read_from(retval);
      bbdo_stream->write_to(retval);
    }
    retval = bbdo_stream;
  }
  return (retval);
}
