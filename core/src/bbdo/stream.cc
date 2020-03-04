/*
** Copyright 2013,2015,2017 Centreon
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

#include "com/centreon/broker/bbdo/stream.hh"

#include <algorithm>

#include "com/centreon/broker/bbdo/ack.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
stream::stream()
    : _coarse(false),
      _negotiate(true),
      _negotiated(false),
      _timeout(5),
      _acknowledged_events(0),
      _ack_limit(1000),
      _events_received_since_last_ack(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
// stream::stream(stream const& other)
//    : io::stream(other),
//      input(other),
//      output(other),
//      _coarse(other._coarse),
//      _extensions(other._extensions),
//      _negotiate(other._negotiate),
//      _negotiated(other._negotiated),
//      _timeout(other._timeout),
//      _acknowledged_events(other._acknowledged_events),
//      _ack_limit(other._ack_limit),
//      _events_received_since_last_ack(other._events_received_since_last_ack)
//      {}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Flush stream data.
 *
 *  @return Number of acknowledged events.
 */
int stream::flush() {
  output::flush();
  int retval(_acknowledged_events);
  _acknowledged_events = 0;
  return (retval);
}

/**
 *  Negotiate features with peer.
 *
 *  @param[in] neg  Negotiation type.
 */
void stream::negotiate(stream::negotiation_type neg) {
  // Coarse peer don't expect any salutation either.
  if (_coarse) {
    _negotiated = true;
    return;
  } else if (_negotiated)
    return;

  // Send our own packet if we should be first.
  if (neg == negotiate_first) {
    log_v2::instance().bbdo()->debug(
        "BBDO: sending welcome packet (available extensions: {})",
        (_negotiate ? _extensions : ""));
    logging::debug(logging::medium)
        << "BBDO: sending welcome packet (available extensions: "
        << (_negotiate ? _extensions : "") << ")";
    std::shared_ptr<version_response> welcome_packet(
        std::make_shared<version_response>());
    if (_negotiate)
      welcome_packet->extensions = _extensions;
    output::write(welcome_packet);
    output::flush();
  }

  // Read peer packet.
  log_v2::instance().bbdo()->debug("BBDO: retrieving welcome packet of peer");
  logging::debug(logging::medium) << "BBDO: retrieving welcome packet of peer";
  std::shared_ptr<io::data> d;
  time_t deadline;
  if (_timeout == (time_t)-1)
    deadline = (time_t)-1;
  else
    deadline = time(nullptr) + _timeout;
  read_any(d, deadline);
  if (!d || (d->type() != version_response::static_type())) {
    log_v2::instance().bbdo()->error(
        "BBDO: invalid protocol header, aborting connection");
    throw(exceptions::msg()
          << "BBDO: invalid protocol header, aborting connection");
  }

  // Handle protocol version.
  std::shared_ptr<version_response> v(
      std::static_pointer_cast<version_response>(d));
  if (v->bbdo_major != BBDO_VERSION_MAJOR) {
    log_v2::instance().bbdo()->error(
        "BBDO: peer is using protocol version {0}.{1}.{2} whereas we're using "
        "protocol version {3}.{4}.{5}",
        v->bbdo_major, v->bbdo_minor, v->bbdo_patch, BBDO_VERSION_MAJOR,
        BBDO_VERSION_MINOR, BBDO_VERSION_PATCH);
    throw(exceptions::msg()
          << "BBDO: peer is using protocol version " << v->bbdo_major << "."
          << v->bbdo_minor << "." << v->bbdo_patch
          << " whereas we're using protocol version " << BBDO_VERSION_MAJOR
          << "." << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH);
  }
  log_v2::instance().bbdo()->info(
      "BBDO: peer is using protocol version {0}.{1}.{2}, we're using version "
      "{3}.{4}.{5}",
      v->bbdo_major, v->bbdo_minor, v->bbdo_patch, BBDO_VERSION_MAJOR,
      BBDO_VERSION_MINOR, BBDO_VERSION_PATCH);
  logging::info(logging::medium)
      << "BBDO: peer is using protocol version " << v->bbdo_major << "."
      << v->bbdo_minor << "." << v->bbdo_patch << ", we're using version "
      << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
      << BBDO_VERSION_PATCH;

  // Send our own packet if we should be second.
  if (neg == negotiate_second) {
    log_v2::instance().bbdo()->debug(
        "BBDO: sending welcome packet (available extensions: {})",
        (_negotiate ? _extensions : ""));
    logging::debug(logging::medium)
        << "BBDO: sending welcome packet (available extensions: "
        << (_negotiate ? _extensions : "") << ")";
    std::shared_ptr<version_response> welcome_packet(
        std::make_shared<version_response>());
    if (_negotiate)
      welcome_packet->extensions = _extensions;
    output::write(welcome_packet);
    output::flush();
  }

  // Negotiation.
  if (_negotiate) {
    // Apply negotiated extensions.
    log_v2::instance().bbdo()->info(
        "BBDO: we have extensions '{0}' and peer has '{1}'", _extensions,
        v->extensions);
    logging::info(logging::medium)
        << "BBDO: we have extensions '" << _extensions << "' and peer has '"
        << v->extensions << "'";
    std::list<std::string> own_ext(misc::string::split(_extensions, ' '));
    std::list<std::string> peer_ext(misc::string::split(v->extensions, ' '));
    for (std::list<std::string>::const_iterator it{own_ext.begin()},
         end{own_ext.end()};
         it != end; ++it) {
      // Find matching extension in peer extension list.
      std::list<std::string>::const_iterator peer_it{
          std::find(peer_ext.begin(), peer_ext.end(), *it)};
      // Apply extension if found.
      if (peer_it != peer_ext.end()) {
        log_v2::instance().bbdo()->info("BBDO: applying extension '{}'", *it);
        logging::info(logging::medium)
            << "BBDO: applying extension '" << *it << "'";
        for (std::map<std::string, io::protocols::protocol>::const_iterator
                 proto_it{io::protocols::instance().begin()},
             proto_end{io::protocols::instance().end()};
             proto_it != proto_end; ++proto_it)
          if (proto_it->first == *it) {
            std::shared_ptr<io::stream> s{
                proto_it->second.endpntfactry->new_stream(
                    _substream, neg == negotiate_second, *it)};
            set_substream(s);
            break;
          }
      }
    }
  }

  // Stream has now negotiated.
  _negotiated = true;
}

/**
 *  Read data from stream.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Deadline.
 *
 *  @return Respect io::stream::read() return value.
 *
 *  @see input::read()
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  d.reset();
  if (!_negotiated)
    negotiate(negotiate_second);
  bool retval = input::read(d, deadline);
  if (retval && d)
    ++_events_received_since_last_ack;
  if (_events_received_since_last_ack >= _ack_limit)
    send_event_acknowledgement();
  return (retval);
}

/**
 *  Set the limit of events received before an ack should be sent.
 *
 *  @param limit  The limit of events received before an ack should be sent.
 */
void stream::set_ack_limit(uint32_t limit) {
  _ack_limit = limit;
}

/**
 *  Set whether this stream is coarse or not.
 *
 *  @param[in] coarse  True if coarse.
 */
void stream::set_coarse(bool coarse) {
  _coarse = coarse;
}

/**
 *  Set whether or not the stream should negotiate features.
 *
 *  @param[in] negotiate   True if the stream should negotiate features.
 *  @param[in] extensions  Extensions supported by this stream.
 */
void stream::set_negotiate(bool negotiate, std::string const& extensions) {
  _negotiate = negotiate;
  _extensions = extensions;
}

/**
 *  Set the timeout supported by this stream.
 *
 *  @param[in] timeout  Timeout in seconds.
 */
void stream::set_timeout(int timeout) {
  _timeout = timeout;
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(json11::Json::object& tree) const {
  tree["bbdo_input_ack_limit"] = static_cast<double>(_ack_limit);
  tree["bbdo_unacknowledged_events"] =
      static_cast<double>(_events_received_since_last_ack);

  output::statistics(tree);
}

/**
 *  Write data to stream.
 *
 *  @param[in] d Data to send.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& d) {
  if (!_negotiated)
    negotiate(negotiate_second);
  output::write(d);
  int retval(_acknowledged_events);
  _acknowledged_events = 0;
  return (retval);
}

/**
 *  Acknowledge a certain amount of events.
 *
 *  @param[in] events  The amount of event.
 */
void stream::acknowledge_events(uint32_t events) {
  _acknowledged_events += events;
}

/**
 *  Send an acknowledgement for all the events received.
 */
void stream::send_event_acknowledgement() {
  if (!_coarse) {
    std::shared_ptr<ack> acknowledgement(std::make_shared<ack>());
    acknowledgement->acknowledged_events = _events_received_since_last_ack;
    output::write(acknowledgement);
    _events_received_since_last_ack = 0;
  }
}
