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

#include "com/centreon/broker/bam/metric_book.hh"
#include "com/centreon/broker/bam/metric_listener.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
metric_book::metric_book() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
metric_book::metric_book(metric_book const& other)
  : _book(other._book) {}

/**
 *  Destructor.
 */
metric_book::~metric_book() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
metric_book& metric_book::operator=(metric_book const& other) {
  if (this != &other)
    _book = other._book;
  return (*this);
}

/**
 *  Make a metric listener listen to metric updates.
 *
 *  @param[in]     metric_id  Metric ID.
 *  @param[in,out] listnr     Metric listener.
 */
void metric_book::listen(
                     unsigned int metric_id,
                     metric_listener* listnr) {
  _book.insert(std::make_pair(metric_id, listnr));
  return ;
}

/**
 *  Remove a listener.
 *
 *  @param[in] metric_id  Metric ID.
 *  @param[in] listnr      Metric listener.
 */
void metric_book::unlisten(
                     unsigned int metric_id,
                     metric_listener* listnr) {
  std::pair<multimap::iterator, multimap::iterator>
    range(_book.equal_range(metric_id));
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
 *  Update all metric listeners related to the metric.
 *
 *  @param[in]  m        Metric status.
 *  @param[out] visitor  Object that will receive events.
 */
void metric_book::update(
                    misc::shared_ptr<storage::metric> const& m,
                    monitoring_stream* visitor) {
  std::pair<multimap::iterator, multimap::iterator>
    range(_book.equal_range(m->metric_id));
  while (range.first != range.second) {
    range.first->second->metric_update(m, visitor);
    ++range.first;
  }
  return ;
}
