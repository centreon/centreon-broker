/*
** Copyright 2009-2012 Centreon
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

#include "com/centreon/broker/config/endpoint.hh"

using namespace com::centreon::broker::config;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
endpoint::endpoint()
  : buffering_timeout(0),
    read_timeout((time_t)-1),
    retry_interval(30) {}

/**
 *  Copy constructor.
 *
 *  @param[in] e Object to copy.
 */
endpoint::endpoint(endpoint const& e) {
  _internal_copy(e);
}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
endpoint& endpoint::operator=(endpoint const& e) {
  _internal_copy(e);
  return (*this);
}

/**
 *  Check that two endpoint configurations are equal.
 *
 *  @param[in] e Object to compare to.
 *
 *  @return true if both objects are equal, false otherwise.
 */
bool endpoint::operator==(endpoint const& e) const {
  return ((type == e.type)
          && (buffering_timeout == e.buffering_timeout)
          && (read_timeout == e.read_timeout)
          && (retry_interval == e.retry_interval)
          && (name == e.name)
          && (failover == e.failover)
          && (filters == e.filters)
          && (params == e.params)
          && (cfg == e.cfg));
}

/**
 *  Check that two endpoint configurations are inequal.
 *
 *  @param[in] e Object to compare to.
 *
 *  @return true if both objects are not equal, false otherwise.
 */
bool endpoint::operator!=(endpoint const& e) const {
  return (!operator==(e));
}

/**
 *  Inequality operator.
 *
 *  @param[in] e Object to compare to.
 *
 *  @return true if this object is strictly less than the object e.
 */
bool endpoint::operator<(endpoint const& e) const {
  // Check properties that can directly be checked.
  if (type != e.type)
    return (type < e.type);
  else if (buffering_timeout != e.buffering_timeout)
    return (buffering_timeout < e.buffering_timeout);
  else if (read_timeout != e.read_timeout)
    return (read_timeout < e.read_timeout);
  else if (retry_interval != e.retry_interval)
    return (retry_interval < e.retry_interval);
  else if (name != e.name)
    return (name < e.name);
  else if (failover != e.failover)
    return (failover < e.failover);
  else if (filters != e.filters)
    return (filters < e.filters);
  else if (cfg != e.cfg)
    return (cfg.toText().data() < e.cfg.toText().data());

  // Need to check all parameters one by one.
  QMap<QString, QString>::const_iterator it1(params.begin()),
    it2(e.params.begin()),
    end1(params.end()),
    end2(e.params.end());
  while ((it1 != end1) && (it2 != end2)) {
    if (it1.key() != it2.key())
      return (it1.key() < it2.key());
    else if (it1.value() != it2.value())
      return (it1.value() < it2.value());
    ++it1;
    ++it2;
  }
  return ((it1 == end1) && (it2 != end2));
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy data members.
 *
 *  Copy all data members from the given object to the current instance.
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] e Object to copy.
 */
void endpoint::_internal_copy(endpoint const& e) {
  buffering_timeout = e.buffering_timeout;
  failover = e.failover;
  name = e.name;
  params = e.params;
  read_timeout = e.read_timeout;
  retry_interval = e.retry_interval;
  filters = e.filters;
  type = e.type;
  cfg = e.cfg;
  return ;
}
