/*
** Copyright 2009-2013 Centreon
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
    retry_interval(30),
    cache_enabled(false) {}

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
  return (*this);
}

/**
 *  Check that two endpoint configurations are equal.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are equal, false otherwise.
 */
bool endpoint::operator==(endpoint const& other) const {
  return ((type == other.type)
          && (buffering_timeout == other.buffering_timeout)
          && (read_timeout == other.read_timeout)
          && (retry_interval == other.retry_interval)
          && (name == other.name)
          && (failovers == other.failovers)
          && (read_filters == other.read_filters)
          && (write_filters == other.write_filters)
          && (params == other.params)
          && (cache_enabled == other.cache_enabled)
          && (cfg == other.cfg));
}

/**
 *  Check that two endpoint configurations are inequal.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are not equal, false otherwise.
 */
bool endpoint::operator!=(endpoint const& other) const {
  return (!operator==(other));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if this object is strictly less than the object e.
 */
bool endpoint::operator<(endpoint const& other) const {
  // Check properties that can directly be checked.
  if (type != other.type)
    return (type < other.type);
  else if (buffering_timeout != other.buffering_timeout)
    return (buffering_timeout < other.buffering_timeout);
  else if (read_timeout != other.read_timeout)
    return (read_timeout < other.read_timeout);
  else if (retry_interval != other.retry_interval)
    return (retry_interval < other.retry_interval);
  else if (name != other.name)
    return (name < other.name);
  else if (failovers != other.failovers)
    return (failovers < other.failovers);
  else if (read_filters != other.read_filters)
    return (read_filters < other.read_filters);
  else if (write_filters != other.write_filters)
    return (write_filters < other.write_filters);
  else if (cache_enabled != other.cache_enabled)
    return (cache_enabled < other.cache_enabled);
  else if (cfg != other.cfg)
    return (cfg < other.cfg);

  // Need to check all parameters one by one.
  QMap<QString, QString>::const_iterator it1(params.begin()),
    it2(other.params.begin()),
    end1(params.end()),
    end2(other.params.end());
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
 *  @param[in] other  Object to copy.
 */
void endpoint::_internal_copy(endpoint const& other) {
  buffering_timeout = other.buffering_timeout;
  failovers = other.failovers;
  name = other.name;
  params = other.params;
  read_filters = other.read_filters;
  read_timeout = other.read_timeout;
  retry_interval = other.retry_interval;
  type = other.type;
  write_filters = other.write_filters;
  cache_enabled = other.cache_enabled;
  cfg = other.cfg;
  return ;
}
