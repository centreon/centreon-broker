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

#ifndef CCB_NOTIFICATION_LOADERS_DEPENDENCY_LOADER_HH
#define CCB_NOTIFICATION_LOADERS_DEPENDENCY_LOADER_HH

#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/dependency_builder.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class dependency_loader dependency_loader.hh
 * "com/centreon/broker/notification/loaders/dependency_loader.hh"
 *  @brief Loader for dependency objects.
 *
 *  This loader loads the dependencies from the database.
 */
class dependency_loader {
 public:
  dependency_loader();

  void load(mysql* db, dependency_builder* output);

 private:
  void _load_relations(mysql* ms, dependency_builder& output);
  //    void   _load_relation(
  //             QSqlQuery& query, dependency_builder& output,
  //             std::string const& relation_id_name,
  //             std::string const& table,
  //             void (dependency_builder::*register_method)
  //                    (unsigned int, unsigned int));
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_LOADERS_DEPENDENCY_LOADER_HH
