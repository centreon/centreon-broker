/*
** Copyright 2014 Merethis
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

#include <QHash>
#include <QPair>
#include "com/centreon/broker/notification/objects/node_id.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
node_id::node_id() :
  _host_id(0),
  _service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
node_id::node_id(node_id const& obj) {
  node_id::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
node_id& node_id::operator=(node_id const& obj) {
  if (this != &obj) {
    _host_id = obj._host_id;
    _service_id = obj._service_id;
  }
  return (*this);
}

/**
 *  Constructor given host and service id.
 *
 *  @param[in] host_id    The host id of this node. 0 if none.
 *  @param[in] service_id  The service id of this node. 0 if none.
 */
node_id::node_id(
           unsigned int host_id,
           unsigned int service_id) :
  _host_id(host_id),
  _service_id(service_id) {}

/**
 *  Comparison operator.
 *
 *  @param obj  The object to compare with.
 *
 *  @return     True if this object is lesser than the other.
 */
bool node_id::operator<(node_id const& obj) const throw() {
  if (_host_id != obj._host_id)
    return (_host_id < obj._host_id);
  else
    return (_service_id < obj._service_id);
}

/**
 *  Equality operator.
 *
 *  @param[in] obj  The object to compare with.
 *
 *  @return         True if this object is equal to the other.
 */
bool node_id::operator==(node_id const& obj) const throw() {
  return (_host_id == obj._host_id && _service_id == obj._service_id);
}

/**
 *  Inequality operator.
 *
 *  @param[in] obj  The object to compare with.
 *
 *  @return         True if the objects are inequal.
 */
bool node_id::operator!=(node_id const& obj) const throw() {
  return (!node_id::operator==(obj));
}

/**
 *  Get the host id of this node. 0 if none.
 *
 *  @return  The host id of this node. 0 if none.
 */
unsigned int node_id::get_host_id() const throw() {
  return (_host_id);
}

/**
 *  Get the service id of this node. 0 if none.
 *
 *  @return  The service id of this node. 0 if none.
 */
unsigned int node_id::get_service_id() const throw() {
  return (_service_id);
}

/**
 *  Is this node a host?
 *
 *  @return  True if this node is a host.
 */
bool node_id::is_host() const throw() {
  return (_service_id == 0);
}

/**
 *  Is this node a service?
 *
 *  @return  True if this node is a service.
 */
bool node_id::is_service() const throw() {
  return (_service_id != 0);
}

/**
 *  QHash function for hash and sets.
 *
 *  @param[in] id  A node id.
 *
 *  @return        An identifier for this node_id usable in hashtable.
 */
uint com::centreon::broker::notification::objects::qHash(node_id id) {
  return (qHash(qMakePair(id.get_host_id(),
                          id.get_service_id())));
}
