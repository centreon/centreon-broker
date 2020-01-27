/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_CONFIG_APPLIER_LOGGER_HH
#define CCB_CONFIG_APPLIER_LOGGER_HH

#include <list>
#include <map>
#include <memory>
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/logging/backend.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace config {
namespace applier {
/**
 *  @class logger logger.hh "com/centreon/broker/config/applier/logger.hh"
 *  @brief Open and close loggers.
 *
 *  According to some configuration, open and close logging objects
 *  as requested.
 */
class logger {
  std::map<config::logger, std::shared_ptr<logging::backend> > _backends;

  logger();
  logger(logger const& l);
  logger& operator=(logger const& l);
  std::shared_ptr<logging::backend> _new_backend(config::logger const& cfg);

 public:
  ~logger();
  void apply(std::list<config::logger> const& loggers);
  static logger& instance();
  static void load();
  static void unload();

};
}  // namespace applier
}  // namespace config

CCB_END()

#endif  // !CCB_CONFIG_APPLIER_LOGGER_HH
