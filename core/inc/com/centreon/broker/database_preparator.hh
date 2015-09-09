/*
** Copyright 2015 Centreon
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

#ifndef CCB_DATABASE_PREPARATOR_HH
#  define CCB_DATABASE_PREPARATOR_HH

#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class database_preparator database_preparator.hh "com/centreon/broker/database_preparator.hh"
 *  @brief Prepare database queries.
 *
 *  Prepare queries using event mappings.
 */
class                  database_preparator {
public:
  typedef std::set<std::string> event_unique;

                       database_preparator(
                         unsigned int event_id,
                         event_unique const& unique = event_unique(),
                         database_query::excluded_fields const& excluded
                         = database_query::excluded_fields());
                       database_preparator(
                         database_preparator const& other);
                       ~database_preparator();
  database_preparator& operator=(database_preparator const& other);
  void                 prepare_insert(database_query& q);
  void                 prepare_update(database_query& q);
  void                 prepare_delete(database_query& q);

private:
  unsigned int         _event_id;
  database_query::excluded_fields
                       _excluded;
  event_unique         _unique;
};

CCB_END()

#endif // !CCB_DATABASE_PREPARATOR_HH
