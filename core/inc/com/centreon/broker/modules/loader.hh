/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_MODULES_LOADER_HH
#define CCB_MODULES_LOADER_HH

#include <map>
#include <memory>
#include <string>
#include "com/centreon/broker/modules/handle.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace modules {
/**
 *  @class loader loader.hh "com/centreon/broker/modules/loader.hh"
 *  @brief Load Centreon Broker plugins.
 *
 *  This class is used to load Centreon Broker plugins and hold
 *  reference to them as long as they exist.
 */
class loader {
  std::map<std::string, std::shared_ptr<handle> > _handles;

 public:
  typedef std::map<std::string, std::shared_ptr<handle> >::iterator
      iterator;

  loader() = default;
  loader(loader const& l) = delete;
  ~loader();
  loader& operator=(loader const& l) = delete;
  iterator begin();
  iterator end();
  void load_dir(std::string const& dirname, void const* arg = nullptr);
  void load_file(std::string const& filename, void const* arg = nullptr);
  void unload();
};
}  // namespace modules

CCB_END()

#endif  // !CCB_MODULES_LOADER_HH
