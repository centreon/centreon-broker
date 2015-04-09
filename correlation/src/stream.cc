/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Constructor.
 *
 *  @param[in]      correlation_file  Correlation file.
 *  @param[int,out] cache             Persistent cache.
 */
stream::stream(
          QString const& correlation_file,
	  misc::shared_ptr<persistent_cache> cache)
  : _correlation_file(correlation_file),
    _process_out(true) {
  _load_correlation();
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Set which data to process.
 *
 *  @param[in] in   Process in.
 *  @param[in] out  Process out.
 */
void stream::process(bool in, bool out) {
  bool was_processing(_process_out);
  _process_out = in || !out; // Only for immediate shutdown.
  if (was_processing && !_process_out)
    _save_persistent_cache();
  return ;
}

/**
 *  Read data from the stream.
 *
 *  @param[out] d  Unused.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
	 << "correlation: cannot read from a stream. This is likely a "
	 << "software bug that you should report to Centreon Broker "
	 << "developers");
  return ;
}

/**
 *  Update the stream.
 */
void stream::update() {
  _save_persistent_cache();
  _load_correlation();
  return ;
}

/**
 *  Write data to the correlation stream.
 *
 *  @param[in] d  Multiplexed data.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data can be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
	   << "correlation stream is shutdown");

  // XXX
}

/**
 *  Load correlation from the configuration file and the persistent
 *  cache.
 */
void stream::_load_correlation() {
  // XXX
}

/**
 *  Save content of the correlation memory in the persistent cache.
 */
void stream::_save_persistent_cache() {
  // XXX
}
