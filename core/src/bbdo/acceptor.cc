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

#include "com/centreon/broker/bbdo/acceptor.hh"

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>

#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**
 *  Constructor.
 *
 *  @param[in] name                    The name to build temporary.
 *  @param[in] negotiate               true if feature negotiation is
 *                                     allowed.
 *  @param[in] extensions              Pair of two strings with extensions the
 *                                     one contains those allowed by this
 *                                     endpoint. The second one contains the
 *                                     mandatory ones.
 *  @param[in] timeout                 Connection timeout.
 *  @param[in] one_peer_retention_mode True to enable the "one peer
 *                                     retention mode" (TM).
 *  @param[in] coarse                  If the acceptor is coarse or not.
 *  @param[in] ack_limit               The number of event received before
 *                                     an ack needs to be sent.
 */
acceptor::acceptor(std::string const& name,
                   bool negotiate,
                   std::pair<std::string, std::string> const& extensions,
                   time_t timeout,
                   bool one_peer_retention_mode,
                   bool coarse,
                   uint32_t ack_limit)
    : io::endpoint(!one_peer_retention_mode),
      _coarse(coarse),
      _extensions(extensions),
      _name(name),
      _negotiate(negotiate),
      _one_peer_retention_mode(one_peer_retention_mode),
      _timeout(timeout),
      _ack_limit(ack_limit) {
  if (_timeout == (time_t)-1 || _timeout == 0)
    _timeout = 3;
}

/**
 *  Destructor.
 */
acceptor::~acceptor() noexcept {
  _from.reset();
}

/**
 *  Wait for incoming connection.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
std::unique_ptr<io::stream> acceptor::open() {
  // Wait for client from the lower layer.
  if (_from) {
    std::unique_ptr<io::stream> u;
    do {
      u = _from->open();
    } while (_one_peer_retention_mode && !u);

    // Add BBDO layer.
    if (u) {
      assert(!_coarse);
      // if _one_peer_retention_mode, the stream is an output
      bbdo::stream* my_bbdo = new bbdo::stream(!_one_peer_retention_mode);
      my_bbdo->set_substream(std::move(u));
      my_bbdo->set_coarse(_coarse);
      my_bbdo->set_negotiate(_negotiate, _extensions);
      my_bbdo->set_timeout(_timeout);
      my_bbdo->set_ack_limit(_ack_limit);
      my_bbdo->negotiate(bbdo::stream::negotiate_second);

      return std::unique_ptr<io::stream>(my_bbdo);
    }
  }

  return std::unique_ptr<io::stream>();
}

/**
 *  Get BBDO statistics.
 *
 *  @param[out] tree Properties tree.
 */
void acceptor::stats(json11::Json::object& tree) {
  tree["one_peer_retention_mode"] = _one_peer_retention_mode;
  if (_from)
    _from->stats(tree);
}
