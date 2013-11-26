/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/stats/config.hh"
#include "com/centreon/broker/stats/metric.hh"

using namespace com::centreon::broker::stats;

/**
 *  Constructor.
 */
config::config() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
config::config(config const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
config::~config() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
config& config::operator=(config const& right) {
  if (this != &right) {
    _dumper_tag = right._dumper_tag;
    _fifo = right._fifo;
    _interval = right._interval;
    _metrics = right._metrics;
  }
  return (*this);
}

/**
 *  Get the dumper tag.
 *
 *  @return The dumper tag.
 */
std::string const& config::get_dumper_tag() const throw () {
  return (_dumper_tag);
}

/**
 *  Get the fifo path.
 *
 *  @return The fifo path.
 */
std::string const& config::get_fifo() const throw () {
  return (_fifo);
}

/**
 *  Get the dump interval
 *
 *  @return The interval in seconds.
 */
unsigned int config::get_interval() const throw () {
  return (_interval);
}

/**
 *  Get the list of metrics
 *
 *  @return The metrics list.
 */
std::list<metric>& config::metrics() throw () {
  return (_metrics);
}

/**
 *  Get the list of metrics.
 *
 *  @return The metrics list.
 */
std::list<metric> const& config::metrics() const throw () {
  return (_metrics);
}

/**
 *  Set the dumper tag.
 *
 *  @param[in] tag The dumper tag.
 */
void config::set_dumper_tag(std::string const& tag) {
  _dumper_tag = tag;
}

/**
 *  Set the fifo path.
 *
 *  @param[in] fifo The fifo path.
 */
void config::set_fifo(std::string const& fifo) {
  _fifo = fifo;
}

/**
 *  Set the dump interval.
 *
 *  @param[in] interval The interval in seconds.
 */
void config::set_interval(unsigned int interval) throw () {
  _interval = (interval > 1 ? interval : 1);
}
