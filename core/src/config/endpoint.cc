/*
** Copyright 2009-2012 Merethis
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
          && (params == e.params));
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
  type = e.type;
  return ;
}
