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

#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/bam/service_listener.hh"
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
service_book::service_book(service_book const& other)
  : _book(other._book) {}

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
void service_book::listen(
                     unsigned int host_id,
                     unsigned int service_id,
                     service_listener* listnr) {
  _book.insert(std::make_pair(
                      std::make_pair(host_id, service_id),
                      listnr));
  return ;
}

/**
 *  Remove a listener.
 *
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 *  @param[in] listnr      Service listener.
 */
void service_book::unlisten(
                     unsigned int host_id,
                     unsigned int service_id,
                     service_listener* listnr) {
  std::pair<multimap::iterator, multimap::iterator>
    range(_book.equal_range(std::make_pair(host_id, service_id)));
  while (range.first != range.second) {
    if (range.first->second == listnr) {
      _book.erase(range.first);
      break ;
    }
    ++range.first;
  }
  return ;
}

/**
 *  Update all service listeners related to the service.
 *
 *  @param[in]  ss       Service status.
 *  @param[out] visitor  Object that will receive events.
 */
void service_book::update(
                     misc::shared_ptr<neb::service_status> const& ss,
                     monitoring_stream* visitor) {
  std::pair<multimap::iterator, multimap::iterator>
    range(_book.equal_range(std::make_pair(
                                   ss->host_id,
                                   ss->service_id)));
  while (range.first != range.second) {
    range.first->second->service_update(ss, visitor);
    ++range.first;
  }
  return ;
}
