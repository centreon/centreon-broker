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

#ifndef CCB_BAM_IMPACT_VALUES_HH
#define CCB_BAM_IMPACT_VALUES_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class impact_values impact_values.hh
 * "com/centreon/broker/bam/impact_values.hh"
 *  @brief Impact values.
 *
 *  Holds together the different values of an impact: nominal impact,
 *  downtime impact, acknowledgement impact.
 */
class impact_values {
 public:
  impact_values(double nominal = 0.0,
                double acknowledgement = 0.0,
                double downtime = 0.0);
  impact_values(impact_values const& other);
  ~impact_values();
  impact_values& operator=(impact_values const& other);
  bool operator==(impact_values const& other) const throw();
  double get_acknowledgement() const;
  double get_downtime() const;
  double get_nominal() const;
  void set_acknowledgement(double acknowledgement);
  void set_downtime(double downtime);
  void set_nominal(double nominal);

 private:
  void _internal_copy(impact_values const& other);

  double _acknowledgement;
  double _downtime;
  double _nominal;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_IMPACT_VALUES_HH
