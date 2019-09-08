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

#ifndef CCB_STATS_CONFIG_HH
#define CCB_STATS_CONFIG_HH

#include <list>
#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace stats {
// Forward declaration.
class metric;

/**
 *  @class config config.hh "com/centreon/broker/stats/config.hh"
 *  @brief Stats module configuration.
 *
 *  Holds the statistics module configuration.
 */
class config {
 public:
  typedef std::vector<std::string> fifo_list;

  config();
  config(config const& right);
  ~config();
  config& operator=(config const& right);
  fifo_list const& get_fifo() const throw();
  void add_fifo(std::string const& fifo);

 private:
  fifo_list _fifos;
};
}  // namespace stats

CCB_END()

#endif  // !CCB_STATS_CONFIG_HH
