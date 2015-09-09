/*
** Copyright 2013 Centreon
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
