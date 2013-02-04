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
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"

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
 *  @param[in] is_in      Connector should act as input.
 *  @param[in] is_out     Connector should act as output.
 *  @param[in] negociate  True if extension negociation is allowed.
 *  @param[in] extensions Available extensions.
 */
connector::connector(
             bool is_in,
             bool is_out,
             bool negociate,
             QString const& extensions)
  : io::endpoint(false),
    _extensions(extensions),
    _is_in(is_in),
    _is_out(is_out),
    _negociate(negociate) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
connector::connector(connector const& right)
  : io::endpoint(right),
    _extensions(right._extensions),
    _is_in(right._is_in),
    _is_out(right._is_out),
    _negociate(right._negociate) {}

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
    _extensions = right._extensions;
    _is_in = right._is_in;
    _is_out = right._is_out;
    _negociate = right._negociate;
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
    misc::shared_ptr<bbdo::stream> bbdo_stream;
    if (!retval.isNull()) {
      if (_is_in) {
        if (_is_out)
          bbdo_stream = misc::shared_ptr<bbdo::stream>(
                                new bbdo::stream(true, true));
        else
          bbdo_stream = misc::shared_ptr<bbdo::stream>(
                                new bbdo::stream(true, false));
      }
      else
        bbdo_stream = misc::shared_ptr<bbdo::stream>(
                              new bbdo::stream(false, true));
      bbdo_stream->read_from(retval);
      bbdo_stream->write_to(retval);
      misc::shared_ptr<version_response>
        welcome_packet(new version_response);
      if (_negociate)
        welcome_packet->extensions = _extensions;
      bbdo_stream->output::write(welcome_packet.staticCast<io::data>());
    }
    retval = bbdo_stream.staticCast<io::stream>();
  }
  return (retval);
}
