/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/processing/multiple_writer.hh"
#include "com/centreon/broker/processing/failover.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
multiple_writer::multiple_writer()
  : _primary_output(NULL),
    _secondary_endpoints(NULL),
    _name("(UNKNOWN)") {

}

/**
 *  Copy constructor.
 *
 *  @param[in] right  The object to copy.
 */
multiple_writer::multiple_writer(multiple_writer const& right) {
  multiple_writer::operator=(right);
}

/**
 *  Destructor.
 */
multiple_writer::~multiple_writer() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right  The object to copy.
 *
 *  @return           A reference to this object.
 */
multiple_writer& multiple_writer::operator=(multiple_writer const& right) {
  if (this != &right) {
    _primary_output = right._primary_output;
    _secondary_outputs = right._secondary_outputs;
    _secondary_endpoints = right._secondary_endpoints;
    _name = right._name;
  }
  return (*this);
}

/**
 *  Set the primary output.
 *
 *  @param[in] output  The output to add.
 */
void multiple_writer::set_primary_output(io::stream *output) {
  _primary_output = output;
}

unsigned int multiple_writer::write(misc::shared_ptr<io::data> const& d) {
  // We return the number of packet processed by the primary output, or
  // the first secondary output if the primary output isn't set.
  unsigned int packet_processed = 1;
  bool have_processed = false;

  if (_primary_output) {
    packet_processed = _primary_output->write(d);
    have_processed = true;
  }

  // We silently catch the exception of the secondary outputs.
  for (std::list<misc::shared_ptr<io::stream> >::iterator
         it(_secondary_outputs.begin()),
         end(_secondary_outputs.end()),
         tmp = it;
       it != end;
       it = tmp) {
    ++tmp;
    try {
      if (!have_processed) {
        packet_processed = (*it)->write(d);
        have_processed = true;
      }
      else
        (*it)->write(d);
    }
    catch (io::exceptions::shutdown const& e) {
      logging::debug(logging::medium)
        << "failover: endpoint '" << _name
        << "' normal termination of a secondary failover endpoint";
      _secondary_outputs.erase(it);
    }
    catch (std::exception const& e) {
      logging::error(logging::medium)

        << "failover: endpoint '" << _name
        << "' a secondary endpoint failed: '" << e.what()
        << "', closing it";
      _secondary_outputs.erase(it);
    }
  }

  return (packet_processed);
}

/**
 *  Register secondary endpoints and open the streams.
 *
 *  @param[in] failover_name        The name of the failover.
 *  @param[in] secondary_endpoints  The secondary endpoints to be registered.
 */
void multiple_writer::register_secondary_endpoints(
       std::string const& failover_name,
       std::vector<misc::shared_ptr<io::endpoint> >& secondary_endpoints) {
    _secondary_endpoints = &secondary_endpoints;
    _secondary_outputs.clear();
    _name = failover_name;
    unsigned int num = 0;
    for (std::vector<misc::shared_ptr<io::endpoint> >::iterator
           it(_secondary_endpoints->begin()),
           end(_secondary_endpoints->end());
         it != end;
         ++it, ++num) {
      logging::debug(logging::medium)
        << "failover: endpoint " << failover_name
        << " opening secondary failover number " << num;
      misc::shared_ptr<io::stream> tmp((*it)->open());
      if (tmp)
        _secondary_outputs.push_back(tmp);
  }
}

/**
 *  Update all the secondary streams.
 */
void multiple_writer::update() {
  for (std::list<misc::shared_ptr<io::stream> >::iterator
         it(_secondary_outputs.begin()),
         end(_secondary_outputs.end());
       it != end;
       ++it)
  (*it)->update();
}
