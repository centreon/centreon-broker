/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/service_book.hh"

#include "com/centreon/broker/bam/service_listener.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
service_book::service_book() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
service_book::service_book(service_book const& other) : _book(other._book) {}

/**
 *  Destructor.
 */
service_book::~service_book() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
service_book& service_book::operator=(service_book const& other) {
  if (this != &other)
    _book = other._book;
  return (*this);
}

/**
 *  Make a service listener listen to service updates.
 *
 *  @param[in]     host_id     Host ID.
 *  @param[in]     service_id  Service ID.
 *  @param[in,out] listnr      Service listener.
 */
void service_book::listen(uint32_t host_id,
                          uint32_t service_id,
                          service_listener* listnr) {
  _book.insert(std::make_pair(std::make_pair(host_id, service_id), listnr));
  return;
}

/**
 *  Remove a listener.
 *
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 *  @param[in] listnr      Service listener.
 */
void service_book::unlisten(uint32_t host_id,
                            uint32_t service_id,
                            service_listener* listnr) {
  std::pair<multimap::iterator, multimap::iterator> range(
      _book.equal_range(std::make_pair(host_id, service_id)));
  while (range.first != range.second) {
    if (range.first->second == listnr) {
      _book.erase(range.first);
      break;
    }
    ++range.first;
  }
  return;
}

/**
 *  Update all service listeners related to the service.
 *
 *  @param[in]  ss       Service status.
 *  @param[out] visitor  Object that will receive events.
 */
void service_book::update(std::shared_ptr<neb::service_status> const& ss,
                          io::stream* visitor) {
  std::pair<multimap::iterator, multimap::iterator> range(
      _book.equal_range(std::make_pair(ss->host_id, ss->service_id)));
  while (range.first != range.second) {
    range.first->second->service_update(ss, visitor);
    ++range.first;
  }
  return;
}

/**
 *  Update all service listeners related to the service on which the
 *  acknowledgement applies.
 *
 *  @param[in]  ack      Acknowledgement.
 *  @param[out] visitor  Object that will receive events.
 */
void service_book::update(std::shared_ptr<neb::acknowledgement> const& ack,
                          io::stream* visitor) {
  std::pair<multimap::iterator, multimap::iterator> range(
      _book.equal_range(std::make_pair(ack->host_id, ack->service_id)));
  while (range.first != range.second) {
    range.first->second->service_update(ack, visitor);
    ++range.first;
  }
  return;
}

/**
 *  Update all service listeners related to the service on which the
 *  downtime applies.
 *
 *  @param[in]  dt       Downtime.
 *  @param[out] visitor  Object that will receive events.
 */
void service_book::update(std::shared_ptr<neb::downtime> const& dt,
                          io::stream* visitor) {
  std::pair<multimap::iterator, multimap::iterator> range(
      _book.equal_range(std::make_pair(dt->host_id, dt->service_id)));
  while (range.first != range.second) {
    range.first->second->service_update(dt, visitor);
    ++range.first;
  }
  return;
}
