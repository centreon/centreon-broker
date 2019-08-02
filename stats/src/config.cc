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
    _fifos = right._fifos;
  }
  return (*this);
}

/**
 *  Get the fifo path.
 *
 *  @return The fifo path.
 */
config::fifo_list const& config::get_fifo() const throw () {
  return (_fifos);
}


/**
 *  Add a fifo.
 *
 *  @param[in] fifo  The path of the fifo.
 *  @param[in] type  The type of the fifo.
 */
void config::add_fifo(std::string const& fifo) {
  _fifos.push_back(fifo);
}

