/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
