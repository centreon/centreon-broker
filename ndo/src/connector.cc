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

#include "multiplexing/publisher.hh"
#include "multiplexing/subscriber.hh"
#include "ndo/connector.hh"
#include "ndo/input.hh"
#include "ndo/output.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

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
  : _is_in(is_in), _is_out(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
  : io::connector(c), _is_in(c._is_in), _is_out(c._is_out) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  _is_in = c._is_in;
  _is_out = c._is_out;
  return (*this);
}

/**
 *  Close the connector.
 */
void connector::close() {
  return ;
}

/**
 *  Connect to a remote NDO connection.
 *
 *  @param[in] ptr Incoming connection.
 */
void connector::connect(QSharedPointer<io::stream> ptr) {
  // In and out objects.
  QSharedPointer<serialization::iserial> in;
  QSharedPointer<serialization::oserial> out;

  // Create input and output objects.
  if (_is_in) {
    in = QSharedPointer<serialization::iserial>(new ndo::input);
    in->read_from(ptr);
    out = QSharedPointer<serialization::oserial>(new multiplexing::publisher);
  }
  else {
    in = QSharedPointer<serialization::iserial>(new multiplexing::subscriber);
    out = QSharedPointer<serialization::oserial>(new ndo::output);
    out->write_to(ptr);
  }

  // Loop through events.
}

/**
 *  Open the connector.
 */
void connector::open() {
  return ;
}
