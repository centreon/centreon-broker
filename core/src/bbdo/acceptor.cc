/*
** Copyright 2013-2015,2017 Centreon
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
 *  @param[in] negotiate               true if feature negotiation is
 *                                     allowed.
 *  @param[in] extensions              List of extensions allowed by
 *                                     this endpoint.
 *  @param[in] timeout                 Connection timeout.
 *  @param[in] one_peer_retention_mode True to enable the "one peer
 *                                     retention mode" (TM).
 *  @param[in] coarse                  If the acceptor is coarse or not.
 *  @param[in] ack_limit               The number of event received before
 *                                     an ack needs to be sent.
 */
acceptor::acceptor(
            std::string const& name,
            bool negotiate,
            QString const& extensions,
            time_t timeout,
            bool one_peer_retention_mode,
            bool coarse,
            unsigned int ack_limit)
  : io::endpoint(!one_peer_retention_mode),
    _coarse(coarse),
    _extensions(extensions),
    _name(name),
    _negotiate(negotiate),
    _one_peer_retention_mode(one_peer_retention_mode),
    _timeout(timeout),
    _ack_limit(ack_limit) {
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
    _negotiate(other._negotiate),
    _one_peer_retention_mode(other._one_peer_retention_mode),
    _timeout(other._timeout),
    _ack_limit(other._ack_limit) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  _from.reset();
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
    _negotiate = other._negotiate;
    _one_peer_retention_mode = other._one_peer_retention_mode;
    _timeout = other._timeout;
    _ack_limit = other._ack_limit;
  }
  return (*this);
}

/**
 *  Wait for incoming connection.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
std::shared_ptr<io::stream> acceptor::open() {
  // Wait for client from the lower layer.
  if (_from) {
    std::shared_ptr<io::stream> s;
    do {
      s = _from->open();
    } while (_one_peer_retention_mode && !s);

    // Add BBDO layer.
    if (s) {
      std::shared_ptr<bbdo::stream> my_bbdo(std::make_shared<bbdo::stream>());
      my_bbdo->set_substream(s);
      my_bbdo->set_coarse(_coarse);
      my_bbdo->set_negotiate(_negotiate, _extensions);
      my_bbdo->set_timeout(_timeout);
      my_bbdo->set_ack_limit(_ack_limit);
      if (_one_peer_retention_mode)
        my_bbdo->negotiate(bbdo::stream::negotiate_second);

      return (my_bbdo);
    }
  }

  return (std::shared_ptr<io::stream>());
}

/**
 *  Get BBDO statistics.
 *
 *  @param[out] tree Properties tree.
 */
void acceptor::stats(io::properties& tree) {
  io::property& p(tree["one_peer_retention_mode"]);
  p.set_name("one_peer_retention_mode");
  p.set_value(_one_peer_retention_mode ? "true" : "false");
  p.set_graphable(false);
  if (_from)
    _from->stats(tree);
  return ;
}
