/*
** Copyright 2011-2012, 2021 Centreon
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

#ifndef CCB_CONFIG_APPLIER_MODULES_HH
#define CCB_CONFIG_APPLIER_MODULES_HH

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "com/centreon/broker/modules/handle.hh"
#include "com/centreon/broker/namespace.hh"

using handle = com::centreon::broker::modules::handle;

CCB_BEGIN()

namespace config {
namespace applier {
/**
 *  @class modules modules.hh "com/centreon/broker/config/applier/modules.hh"
 *  @brief Load necessary modules.
 *
 *  Load modules as per the configuration.
 */
class modules {
  std::map<std::string, std::shared_ptr<handle>> _handles;
  std::mutex _m_modules;

 public:
  typedef std::map<std::string, std::shared_ptr<handle>>::iterator iterator;

  modules() = default;
  ~modules() noexcept = default;
  modules(const modules&) = delete;
  modules& operator=(const modules&) = delete;
  void apply(const std::list<std::string>& module_list,
             const std::string& module_dir,
             const void* arg = nullptr);
  iterator begin();
  iterator end();
  void discard();

  std::mutex& module_mutex();
  void load_dir(const std::string& dirname, const void* arg);
  void load_file(const std::string& filename, const void* arg);
};
}  // namespace applier
}  // namespace config

CCB_END()

#endif  // !CCB_CONFIG_APPLIER_MODULES_HH
