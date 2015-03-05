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

#include <arpa/inet.h>
#include <stdint.h>
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/output.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Serialize an event in the BBDO protocol.
 *
 *  @param[out] data  Serialized event.
 *  @param[in]  e     Event to serialize.
 *  @param[in]  info  Event information.
 */
static void serialize(
              QByteArray& data,
              io::data const& e,
              bbdo_mapped_type const& mapping_info) {
  unsigned int beginning(data.size());
  data.resize(data.size() + BBDO_HEADER_SIZE);
  *(static_cast<uint32_t*>(static_cast<void*>(data.data() + data.size())) - 1)
    = htonl(e.type());
  mapping::entry const*
    current_entry(mapping_info.mapped_type->get_mapping());
  for (std::vector<getter_setter>::const_iterator
         it(mapping_info.bbdo_entries.begin()),
         end(mapping_info.bbdo_entries.end());
       it != end;
       ++it, ++current_entry) {
    // Skip 0 numbered entries.
    for (;!current_entry->is_null() && current_entry->get_number() == 0;
         ++current_entry);

    // Serialization itself.
    (*it->getter)(e, *current_entry, data);

    // Packet splitting.
    if (static_cast<unsigned int>(data.size())
        >= (beginning + BBDO_HEADER_SIZE + 0xFFFF)) {
      // Set size.
      *(static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning)) + 1)
        = 0xFFFF;

      // Set checksum.
      uint16_t chksum(qChecksum(
                        data.data() + beginning + 2,
                        BBDO_HEADER_SIZE - 2));
      *static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning))
        = htons(chksum);

      // Create new header.
      beginning += BBDO_HEADER_SIZE + 0xFFFF;
      char header[BBDO_HEADER_SIZE];
      memset(header, 0, sizeof(header));
      *static_cast<uint32_t*>(static_cast<void*>(header + 4))
        = htonl(e.type());
      data.insert(beginning, header, sizeof(header));
    }
  }

  // Set (last) packet size.
  *(static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning)) + 1)
    = htons(data.size() - beginning - BBDO_HEADER_SIZE);

  // Checksum.
  uint16_t chksum(qChecksum(
                    data.data() + beginning + 2,
                    BBDO_HEADER_SIZE - 2));
  *static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning))
    = htons(chksum);

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
output::output() : _process_out(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
output::output(output const& other)
  : io::stream(other), _process_out(other._process_out) {}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
output& output::operator=(output const& other) {
  if (this != &other) {
    io::stream::operator=(other);
    _process_out = other._process_out;
  }
  return (*this);
}

/**
 *  Enable or disable output processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output processing.
 */
void output::process(bool in, bool out) {
  (void)in;
  _process_out = out;
  return ;
}

/**
 *  Read data.
 *
 *  @param[out] d Cleared.
 */
void output::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
         << "BBDO: attempt to read from an output object");
  return ;
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void output::statistics(io::properties& tree) const {
  if (!_to.isNull())
    _to->statistics(tree);
  return ;
}

/**
 *  Send an event.
 *
 *  @param[in] e Event to send.
 *
 *  @return Number of events acknowledged.
 */
unsigned int output::write(misc::shared_ptr<io::data> const& e) {
  // Check if data should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, !_process_out)
           << "BBDO: output stream is shutdown");

  // Check if data exists.
  if (!e.isNull()) {
    if (_write_event(e) == false) {
      unsigned int event_type(e->type());
      // BBDO doesn't know this event.
      // Reload all the event mappings and try again.
      logging::debug(logging::medium)
        << "BBDO: unknown event type " << event_type
        << ": reloading mappings and retrying";
      create_mappings();

      if (_write_event(e) == false)
        logging::debug(logging::medium)
          << "BBDO: could not serialize event of type " << event_type
          << " (category = " << io::events::category_of_type(event_type)
          << ", element = " << io::events::element_of_type(event_type) << ")"
          << ": event mapping was not found";
    }
  }
  else
    _to->write(e);

  return (1);
}

/**
 *  Attempt to write an event.
 *
 *  @param[in] e  The event.
 *
 *  @return       True if the event is known.
 */
bool output::_write_event(misc::shared_ptr<io::data> const& e) {
  unsigned int event_type(e->type());
  // Find routine.
  umap<unsigned int, bbdo_mapped_type>::const_iterator
    mapped_type(bbdo_mapping.find(event_type));
  if (mapped_type != bbdo_mapping.end()) {
    logging::debug(logging::medium)
      << "BBDO: serializing event of type " << event_type;
    misc::shared_ptr<io::raw> data(new io::raw);
    serialize(*data, *e, mapped_type->second);
    logging::debug(logging::medium) << "BBDO: event of type "
      << event_type << " successfully serialized in "
      << data->size() << " bytes";
    _to->write(data);
    return (true);
  }
  else
    return (false);
}
