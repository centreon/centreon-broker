/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/notification/objects/node_id.hh"
#include <QHash>
#include <QPair>

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
node_id::node_id() : _host_id(0), _service_id(0) {}

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
node_id::node_id(uint32_t host_id, uint32_t service_id)
    : _host_id(host_id), _service_id(service_id) {}

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
uint32_t node_id::get_host_id() const throw() {
  return (_host_id);
}

/**
 *  Get the service id of this node. 0 if none.
 *
 *  @return  The service id of this node. 0 if none.
 */
uint32_t node_id::get_service_id() const throw() {
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
 *  Return the node_id of the host associated with this node.
 *
 *  @return  The node_id of the host associated with this node.
 */
node_id node_id::to_host() const throw() {
  return (node_id(_host_id));
}

/**
 *  QHash function for hash and sets.
 *
 *  @param[in] id  A node id.
 *
 *  @return        An identifier for this node_id usable in hashtable.
 */
uint com::centreon::broker::notification::objects::qHash(node_id id) {
  return (qHash(qMakePair(id.get_host_id(), id.get_service_id())));
}
