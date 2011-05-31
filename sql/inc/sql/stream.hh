/*
** Copyright 2011 Merethis
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

#ifndef CCB_SQL_STREAM_HH_
# define CCB_SQL_STREAM_HH_

# include <memory>
# include <QSqlDatabase>
# include <QString>
# include <string>
# include <vector>
# include "events/events.hh"
# include "io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        sql {
        /**
         *  @class stream stream.hh "sql/stream.hh"
         *  @brief SQL stream.
         *
         *  Stream events into SQL database.
         */
        class          stream : public com::centreon::broker::io::stream {
         private:
          static void                    (stream::* processing_table[])(events::event const&);
          std::auto_ptr<QSqlQuery>       _acknowledgement_stmt;
          std::auto_ptr<QSqlQuery>       _comment_stmt;
          std::auto_ptr<QSqlQuery>       _custom_variable_insert_stmt;
          std::auto_ptr<QSqlQuery>       _custom_variable_status_stmt;
          std::auto_ptr<QSqlQuery>       _downtime_stmt;
          std::auto_ptr<QSqlQuery>       _event_handler_stmt;
          std::auto_ptr<QSqlQuery>       _flapping_status_stmt;
          std::auto_ptr<QSqlQuery>       _host_stmt;
          std::auto_ptr<QSqlQuery>       _host_check_stmt;
          std::auto_ptr<QSqlQuery>       _host_state_stmt;
          std::auto_ptr<QSqlQuery>       _host_status_stmt;
          std::auto_ptr<QSqlQuery>       _instance_stmt;
          std::auto_ptr<QSqlQuery>       _instance_status_stmt;
          std::auto_ptr<QSqlQuery>       _issue_stmt;
          std::auto_ptr<QSqlQuery>       _notification_stmt;
          std::auto_ptr<QSqlQuery>       _service_insert_stmt;
          std::auto_ptr<QSqlQuery>       _service_stmt;
          std::auto_ptr<QSqlQuery>       _service_check_stmt;
          std::auto_ptr<QSqlQuery>       _service_state_stmt;
          std::auto_ptr<QSqlQuery>       _service_status_stmt;
          QSqlDatabase _db;
          stream&      operator=(stream const& s);
          void         _clean_tables(int instance_id);
          template     <typename T>
          bool         _insert(T const& t);
          void         _prepare();
          template     <typename T>
          bool         _prepare_insert(std::auto_ptr<QSqlQuery>& st);
          template     <typename T>
          bool         _prepare_update(std::auto_ptr<QSqlQuery>& st,
                         std::vector<QString> const& id);
          void         _process_acknowledgement(events::event const& e);
          void         _process_comment(events::event const& e);
          void         _process_custom_variable(events::event const& e);
          void         _process_custom_variable_status(events::event const& e);
          void         _process_downtime(events::event const& e);
          void         _process_event_handler(events::event const& e);
          void         _process_flapping_status(events::event const& e);
          void         _process_host(events::event const& e);
          void         _process_host_check(events::event const& e);
          void         _process_host_dependency(events::event const& e);
          void         _process_host_group(events::event const& e);
          void         _process_host_group_member(events::event const& e);
          void         _process_host_parent(events::event const& e);
          void         _process_host_state(events::event const& e);
          void         _process_host_status(events::event const& e);
          void         _process_instance(events::event const& e);
          void         _process_instance_status(events::event const& e);
          //void         _process_issue(events::event const& e);
          //void         _process_issue_parent(events::event const& e);
          void         _process_log(events::event const& e);
          void         _process_module(events::event const& e);
          void         _process_notification(events::event const& e);
          void         _process_service(events::event const& e);
          void         _process_service_check(events::event const& e);
          void         _process_service_dependency(events::event const& e);
          void         _process_service_group(events::event const& e);
          void         _process_service_group_member(events::event const& e);
          void         _process_service_state(events::event const& e);
          void         _process_service_status(events::event const& e);

         public:
                       stream(QString const& type,
                              QString const& host,
                              QString const& user,
                              QString const& password,
                              QString const& db);
                       stream(stream const& s);
                       ~stream();
          QSharedPointer<com::centreon::broker::io::data>
                       read();
          void         write(QSharedPointer<com::centreon::broker::io::data> d);
        };
      }
    }
  }
}

#endif /* !CCB_SQL_STREAM_HH_ */
