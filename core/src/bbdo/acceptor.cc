/*
** Copyright 2013-2015 Merethis
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
#include <sstream>
#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
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
 *  @param[in] name                    The name to build temporary.
 *  @param[in] negociate               true if feature negociation is
 *                                     allowed.
 *  @param[in] extensions              List of extensions allowed by
 *                                     this endpoint.
 *  @param[in] timeout                 Connection timeout.
 *  @param[in] one_peer_retention_mode True to enable the "one peer
 *                                     retention mode" (TM).
 *  @param[in] coarse                  If the acceptor is coarse or not.
 */
acceptor::acceptor(
            QString const& name,
            bool negociate,
            QString const& extensions,
            time_t timeout,
            bool one_peer_retention_mode,
            bool coarse)
  : io::endpoint(!one_peer_retention_mode),
    _coarse(coarse),
    _extensions(extensions),
    _name(name),
    _negociate(negociate),
    _one_peer_retention_mode(one_peer_retention_mode),
    _timeout(timeout) {
  if ((_timeout == (time_t)-1) || (_timeout == 0))
    _timeout = 3;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
acceptor::acceptor(acceptor const& other)
  : io::endpoint(other),
    _coarse(other._coarse),
    _extensions(other._extensions),
    _name(other._name),
    _negociate(other._negociate),
    _one_peer_retention_mode(other._one_peer_retention_mode),
    _timeout(other._timeout) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  _from.clear();
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _coarse = other._coarse;
    _extensions = other._extensions;
    _name = other._name;
    _negociate = other._negociate;
    _one_peer_retention_mode = other._one_peer_retention_mode;
    _timeout = other._timeout;
  }
  return (*this);
}

/**
 *  Wait for incoming connection.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
misc::shared_ptr<io::stream> acceptor::open() {
  // Wait for client from the lower layer.
  if (!_from.isNull()) {
    misc::shared_ptr<io::stream> s;
    do {
      s = _from->open();
    } while (_one_peer_retention_mode && s.isNull());

    // Add BBDO layer.
    if (!s.isNull()) {
      misc::shared_ptr<bbdo::stream> my_bbdo(new bbdo::stream);
      my_bbdo->set_substream(s);
      my_bbdo->set_coarse(_coarse);
      my_bbdo->set_negociate(_negociate, _extensions);
      my_bbdo->set_timeout(_timeout);
      if (_one_peer_retention_mode)
        my_bbdo->negociate(bbdo::stream::negociate_second);
      return (my_bbdo);
    }
  }

  return (misc::shared_ptr<io::stream>());
}

/**
 *  Get BBDO statistics.
 *
 *  @param[out] tree Properties tree.
 */
void acceptor::stats(io::properties& tree) {
  io::property& p(tree["one_peer_retention_mode"]);
  p.set_perfdata(_one_peer_retention_mode
                 ? "one peer retention mode=true"
                 : "one peer retention mode=false");
  p.set_graphable(false);
  if (!_from.isNull())
    _from->stats(tree);
  return ;
}
