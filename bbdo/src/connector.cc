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

#include <algorithm>
#include <memory>
#include <QStringList>
#include "com/centreon/broker/bbdo/connector.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

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
 *  @param[in] timeout    Timeout.
 */
connector::connector(
             bool is_in,
             bool is_out,
             bool negociate,
             QString const& extensions,
             time_t timeout)
  : io::endpoint(false),
    _extensions(extensions),
    _is_in(is_in),
    _is_out(is_out),
    _negociate(negociate),
    _timeout(timeout) {
  if ((_timeout == (time_t)-1) || (_timeout == 0))
    _timeout = 3;
}

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
    _negociate(right._negociate),
    _timeout(right._timeout) {}

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
    _timeout = right._timeout;
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
  // Return value.
  misc::shared_ptr<io::stream> retval;

  // We must have a lower layer.
  if (!_from.isNull()) {
    // Open lower layer connection and add our own layer.
    retval = _open(_from->open());
  }
  return (retval);
}

/**
 *  Open the connector.
 *
 *  @return Open stream.
 */
misc::shared_ptr<io::stream> connector::open(QString const& id) {
  // Return value.
  misc::shared_ptr<io::stream> retval;

  // We must have a lower layer.
  if (!_from.isNull()) {
    // Open lower layer connection and add our own layer.
    retval = _open(_from->open(id));
  }
  return (retval);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Open the connector.
 *
 *  @return Open stream.
 */
misc::shared_ptr<io::stream> connector::_open(
  misc::shared_ptr<io::stream> stream) {
  misc::shared_ptr<bbdo::stream> bbdo_stream;
  if (!stream.isNull()) {
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
    bbdo_stream->read_from(stream);
    bbdo_stream->write_to(stream);

    // Write welcome packet.
    misc::shared_ptr<version_response>
      welcome_packet(new version_response);
    if (_negociate)
      welcome_packet->extensions = _extensions;
    bbdo_stream->output::write(welcome_packet.staticCast<io::data>());
    bbdo_stream->output::write(misc::shared_ptr<io::data>());

    // Version packet (first packet).
    misc::shared_ptr<io::data> d;
    bbdo_stream->read_any(d, time(NULL) + _timeout);
    if (d.isNull()
        || (d->type()
            != io::data::data_type(io::data::bbdo, bbdo::de_version_response)))
      throw (exceptions::msg() << "BBDO: invalid protocol header, "
             << "aborting connection");
    welcome_packet = d.staticCast<version_response>();
    if (welcome_packet->bbdo_major != BBDO_VERSION_MAJOR)
      throw (exceptions::msg()
             << "BBDO: peer is using protocol version "
             << welcome_packet->bbdo_major << "."
             << welcome_packet->bbdo_minor << "."
             << welcome_packet->bbdo_patch
             << ", whereas we're using version "
             << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR
             << "." << BBDO_VERSION_PATCH);
    logging::info(logging::medium)
      << "BBDO: peer is using protocol version "
      << welcome_packet->bbdo_major << "."
      << welcome_packet->bbdo_minor << "."
      << welcome_packet->bbdo_patch << ", we're using version "
      << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
      << BBDO_VERSION_PATCH;

    // Negociate.
    if (_negociate) {
      // Apply negociated extensions.
      logging::info(logging::medium)
        << "BBDO: peer has extensions '"
        << welcome_packet->extensions << "' and we have '"
        << _extensions << "'";
      QStringList own_ext(_extensions.split(' '));
      QStringList peer_ext(welcome_packet->extensions.split(' '));
      for (QStringList::const_iterator
             it(peer_ext.begin()),
             end(peer_ext.end());
           it != end;
           ++it) {
        // Find matching extension in peer extension list.
        QStringList::const_iterator
          own_it(std::find(own_ext.begin(), own_ext.end(), *it));
        // Apply extension if found.
        if (own_it != own_ext.end()) {
          logging::info(logging::medium)
            << "BBDO: applying extension '" << *it << "'";
          for (QMap<QString, io::protocols::protocol>::const_iterator
                 proto_it(io::protocols::instance().begin()),
                 proto_end(io::protocols::instance().end());
               proto_it != proto_end;
               ++proto_it)
            if (proto_it.key() == *it) {
              misc::shared_ptr<io::stream>
                s(proto_it->endpntfactry->new_stream(
                                            stream,
                                            false,
                                            *it));
              bbdo_stream->read_from(s);
              bbdo_stream->write_to(s);
              break ;
            }
        }
      }
    }
  }
  stream = bbdo_stream.staticCast<io::stream>();
  return (stream);
}
