/*
** Copyright 2011-2013 Centreon
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
#include "com/centreon/broker/io/endpoint.hh"
#include <iostream>
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker::io;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 *
 *  @param[in] is_accptr  True if endpoint is an acceptor.
 */
endpoint::endpoint(bool is_accptr) : _is_acceptor(is_accptr) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
endpoint::endpoint(endpoint const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
endpoint& endpoint::operator=(endpoint const& other) {
  if (this != &other)
    _internal_copy(other);
  return *this;
}

/**
 *  Set the lower layer endpoint object of this endpoint.
 *
 *  @param[in] endp Lower layer endpoint object.
 */
void endpoint::from(std::shared_ptr<endpoint> endp) {
  _from = endp;
  if (_from)
    _from->set_filter(_filter);
}

/**
 *  Check if this endpoint is an acceptor.
 *
 *  @return true if endpoint is an acceptor.
 */
bool endpoint::is_acceptor() const throw() {
  return _is_acceptor;
}

/**
 *  Check if this endpoint is a connector.
 *
 *  @return true if endpoint is a connector.
 */
bool endpoint::is_connector() const throw() {
  return !_is_acceptor;
}

/**
 *  Generate statistics about the endpoint.
 *
 *  @param[out] tree Properties tree.
 */
void endpoint::stats(json11::Json::object& tree) {
  if (_from)
    _from->stats(tree);
}

/**
 *  Set the filter used by this endpoint.
 *
 *  For connector endpoints, the filters are already set by the failover.
 *  Acceptor manage subscribers manually and needs to apply
 *  the filters themselves.
 *
 *  @param[in] filter  The filter.
 */
void endpoint::set_filter(std::set<unsigned int> const& filter) {
  _filter = filter;
  if (_from)
    _from->set_filter(filter);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void endpoint::_internal_copy(endpoint const& other) {
  _from = other._from;
  _is_acceptor = other._is_acceptor;
  return;
}
