/*
** Copyright 2019 Centreon
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
#ifndef CCB_SQL_CONFLICT_MANAGER_HH
#define CCB_SQL_CONFLICT_MANAGER_HH
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()
namespace sql {
class conflict_manager {
  static conflict_manager _singleton;
  conflict_manager();

 public:
  static conflict_manager& instance();
};
}  // namespace sql
CCB_END()

#endif /* !CCB_SQL_CONFLICT_MANAGER_HH */
