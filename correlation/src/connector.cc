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

#include "com/centreon/broker/correlation/connector.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Constructor.
 *
 *  @param[in]     correlation_file  Correlation file.
 *  @param[in]     passive           True if endpoint must be in passive
 *                                   mode.
 *  @param[in,out] cache             Endpoint persistent cache.
 */
connector::connector(
             QString const& correlation_file,
             bool passive,
             misc::shared_ptr<persistent_cache> cache)
  : io::endpoint(false),
    _cache(cache),
    _correlation_file(correlation_file),
    _passive(passive) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other)
  : io::endpoint(other),
    _cache(other._cache),
    _correlation_file(other._correlation_file),
    _passive(other._passive) {}

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
    _cache = other._cache;
    _correlation_file = other._correlation_file;
    _passive = other._passive;
  }
  return (*this);
}

/**
 *  Open a stream.
 *
 *  @return A newly opened stream.
 */
misc::shared_ptr<io::stream> connector::open() {
  // XXX : passive stream
  return (new stream(_correlation_file, _cache, true, _passive));
}
