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

#ifndef CCB_NEB_STATISTICS_PLUGIN_HH
#define CCB_NEB_STATISTICS_PLUGIN_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
namespace statistics {
/**
 *  @class plugin plugin.hh "com/centreon/broker/neb/statistics/plugin.hh"
 *  @brief Interface to execute plugin statistics.
 */
class plugin {
 public:
  plugin(std::string const& name);
  plugin(plugin const& right);
  virtual ~plugin();
  plugin& operator=(plugin const& right);
  virtual std::string const& name() const throw();
  virtual void run(std::string& output, std::string& perfdata) = 0;

 protected:
  std::string _name;
};
}  // namespace statistics
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_STATISTICS_PLUGIN_HH
