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

#ifndef CCB_NOTIFICATION_LOADERS_CONTACT_LOADER_HH
#  define CCB_NOTIFICATION_LOADERS_CONTACT_LOADER_HH

#  include <string>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include "com/centreon/broker/notification/builders/contact_builder.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       notification {

  class  contact_loader {
  public:
         contact_loader();

    void load(QSqlDatabase* db, contact_builder* output);

  private:
    void _load_relations(QSqlQuery& query,
                         contact_builder& output);
    void _load_relation(QSqlQuery& query,
                        contact_builder& output,
                        std::string const& first_relation_id_name,
                        std::string const& second_relation_id_name,
                        std::string const& table,
                        void (contact_builder::*register_method)
                        (unsigned int, unsigned int));
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_LOADERS_CONTACT_LOADER_HH
