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

#ifndef CCB_NOTIFICATION_LOADERS_ESCALATION_LOADER_HH
#define CCB_NOTIFICATION_LOADERS_ESCALATION_LOADER_HH

#include <QSqlDatabase>
#include <QSqlQuery>
#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/escalation_builder.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class escalation_loader escalation_loader.hh
 * "com/centreon/broker/notification/loaders/escalation_loader.hh"
 *  @brief Loader for escalation objects.
 *
 *  This loader loads the escalations from the database.
 */
class escalation_loader {
 public:
  escalation_loader();

  void load(QSqlDatabase* db, escalation_builder* output);

 private:
  std::vector<std::string> _rows;

  void _load_relations(QSqlQuery& query, escalation_builder& output);
  void _load_relation(
      QSqlQuery& query,
      escalation_builder& output,
      std::string const& relation_id_name,
      std::string const& table,
      void (escalation_builder::*register_method)(uint32_t, uint32_t));
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_LOADERS_ESCALATION_LOADER_HH
