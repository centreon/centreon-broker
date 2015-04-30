/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_LOADERS_ESCALATION_LOADER_HH
#  define CCB_NOTIFICATION_LOADERS_ESCALATION_LOADER_HH

#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/escalation_builder.hh"

CCB_BEGIN()

namespace       notification {
  /**
   *  @class escalation_loader escalation_loader.hh "com/centreon/broker/notification/loaders/escalation_loader.hh"
   *  @brief Loader for escalation objects.
   *
   *  This loader loads the escalations from the database.
   */
  class         escalation_loader {
  public:
    escalation_loader();

    void        load(QSqlDatabase* db, escalation_builder* output);

  private:
    std::vector<std::string> _rows;

    void        _load_relations(QSqlQuery& query,
                                escalation_builder& output);
    void        _load_relation(QSqlQuery& query,
                               escalation_builder& output,
                               std::string const& relation_id_name,
                               std::string const& table,
                               void (escalation_builder::*register_method)
                               (unsigned int, unsigned int));
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_LOADERS_ESCALATION_LOADER_HH
