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
  : _primary_output(NULL) {

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

/**
 *  Add a secondary output to the list of output.
 *
 *  @param[in] output  The output to add.
 */
void multiple_writer::add_secondary_output(io::stream *output) {
  if (output)
    _secondary_outputs.push_back(output);
}

unsigned int multiple_writer::write(misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return (1);

  // We return the number of packet processed by the primary output, or
  // the first secondary output if the primary output isn't set.
  unsigned int packet_processed = 1;
  bool have_processed = false;

  if (_primary_output) {
    packet_processed = _primary_output->write(d);
    have_processed = true;
  }
  for (std::vector<io::stream*>::iterator
         it(_secondary_outputs.begin()),
         end(_secondary_outputs.end());
       it != end;
       ++it) {
    if (!have_processed) {
      packet_processed = (*it)->write(d);
      have_processed = true;
    }
    else
      (*it)->write(d);
  }

  return (packet_processed);
}
