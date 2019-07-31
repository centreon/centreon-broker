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
#include "com/centreon/broker/bbdo/connector.hh"
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
 *  @param[in] negotiate  True if extension negotiation is allowed.
 *  @param[in] extensions Available extensions.
 *  @param[in] timeout    Timeout.
 *  @param[in] coarse     Is this connection coarse?
 *  @param[in] ack_limit  The number of event received before an ack needs to be sent.
 */
connector::connector(
             bool negotiate,
             std::string const& extensions,
             time_t timeout,
             bool coarse,
             unsigned int ack_limit)
  : io::endpoint{false},
    _coarse{coarse},
    _extensions{extensions},
    _negotiate{negotiate},
    _timeout{timeout},
    _ack_limit{ack_limit} {
  if (_timeout == (time_t)-1 || _timeout == 0)
    _timeout = 3;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
connector::connector(connector const& other)
  : io::endpoint{other},
    _coarse{other._coarse},
    _extensions{other._extensions},
    _negotiate{other._negotiate},
    _timeout{other._timeout},
    _ack_limit{other._ack_limit} {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _coarse = other._coarse;
    _extensions = other._extensions;
    _negotiate = other._negotiate;
    _timeout = other._timeout;
    _ack_limit = other._ack_limit;
  }
  return (*this);
}

/**
 *  Open the connector.
 *
 *  @return Open stream.
 */
std::shared_ptr<io::stream> connector::open() {
  // Return value.
  std::shared_ptr<io::stream> retval;

  // We must have a lower layer.
  if (_from) {
    // Open lower layer connection and add our own layer.
    retval = _open(_from->open());
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
std::shared_ptr<io::stream> connector::_open(
                               std::shared_ptr<io::stream> stream) {
  std::shared_ptr<bbdo::stream> bbdo_stream;
  if (stream) {
    bbdo_stream = std::make_shared<bbdo::stream>();
    bbdo_stream->set_substream(stream);
    bbdo_stream->set_coarse(_coarse);
    bbdo_stream->set_negotiate(_negotiate, _extensions);
    bbdo_stream->set_timeout(_timeout);
    bbdo_stream->negotiate(bbdo::stream::negotiate_first);
    bbdo_stream->set_ack_limit(_ack_limit);
  }
  return bbdo_stream;
}
