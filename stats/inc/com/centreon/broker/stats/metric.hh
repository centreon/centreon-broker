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

#ifndef CCB_STATS_METRIC_HH
#define CCB_STATS_METRIC_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace stats {
/**
 *  @class metric metric.hh "com/centreon/broker/stats/metric.hh"
 *  @brief Configuration of a metric.
 *
 *  Holds parameters of a statistic metric.
 */
class metric {
 public:
  metric();
  metric(metric const& right);
  ~metric();
  metric& operator=(metric const& right);
  unsigned int get_host_id() const throw();
  std::string const& get_name() const throw();
  unsigned int get_service_id() const throw();
  void set_host_id(unsigned int host_id) throw();
  void set_name(std::string const& name);
  void set_service_id(unsigned int service_id) throw();

 private:
  unsigned int _host_id;
  std::string _name;
  unsigned int _service_id;
};
}  // namespace stats

CCB_END()

#endif  // !CCB_STATS_METRIC_HH
