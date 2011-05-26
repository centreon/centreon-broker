/*
** Copyright 2011 Merethis
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

#include <assert.h>
#include <stdlib.h>
#include "logging/logging.hh"
#include "processing/failover.hh"

using namespace com::centreon::broker::processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] f Object to copy.
 */
failover::failover(failover const& f) : QThread() {
  (void)f;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
failover& failover::operator=(failover const& f) {
  (void)f;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
failover::failover() {}

/**
 *  Destructor.
 */
failover::~failover() {}

/**
 *  Run the failover thread.
 */
void failover::run() {
  // Check that endpoint is not empty.
  if (_endpoint.isNull()) {
    logging::error << logging::HIGH << "thread launched with no valid endpoint";
    return ;
  }

  // Launch processing.
  // XXX : need exiting method
}

/**
 *  Set the endpoint this thread will work on.
 *
 *  @param[in] endp Thread endpoint.
 *  @param[in] m    Processing mode (input or output).
 */
void failover::set_endpoint(QSharedPointer<com::centreon::broker::io::endpoint> endp,
                            failover::mode m) {
  _endpoint = endp;
  _mode = m;
  return ;
}
