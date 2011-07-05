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
# include <QHash>
# include <QSqlDatabase>
# include <QString>
# include <string>
# include <vector>
# include "com/centreon/broker/io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        sql {
        /**
         *  @class stream stream.hh "com/centreon/broker/sql/stream.hh"
         *  @brief SQL stream.
         *
         *  Stream events into SQL database.
         */
        class          stream : public io::stream {
         private:
          static QHash<QString, void (stream::*)(io::data const&)>
                                         processing_table;
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
          void         _execute(QString const& query);
          void         _execute(QSqlQuery& query);
          template     <typename T>
          bool         _insert(T const& t);
          void         _prepare();
          template     <typename T>
          bool         _prepare_insert(std::auto_ptr<QSqlQuery>& st);
          template     <typename T>
          bool         _prepare_update(std::auto_ptr<QSqlQuery>& st,
                         std::vector<QString> const& id);
          void         _process_acknowledgement(io::data const& e);
          void         _process_comment(io::data const& e);
          void         _process_custom_variable(io::data const& e);
          void         _process_custom_variable_status(io::data const& e);
          void         _process_downtime(io::data const& e);
          void         _process_event_handler(io::data const& e);
          void         _process_flapping_status(io::data const& e);
          void         _process_host(io::data const& e);
          void         _process_host_check(io::data const& e);
          void         _process_host_dependency(io::data const& e);
          void         _process_host_group(io::data const& e);
          void         _process_host_group_member(io::data const& e);
          void         _process_host_parent(io::data const& e);
          void         _process_host_state(io::data const& e);
          void         _process_host_status(io::data const& e);
          void         _process_instance(io::data const& e);
          void         _process_instance_status(io::data const& e);
          void         _process_issue(io::data const& e);
          void         _process_issue_parent(io::data const& e);
          void         _process_log(io::data const& e);
          void         _process_module(io::data const& e);
          void         _process_nothing(io::data const& e);
          void         _process_notification(io::data const& e);
          void         _process_service(io::data const& e);
          void         _process_service_check(io::data const& e);
          void         _process_service_dependency(io::data const& e);
          void         _process_service_group(io::data const& e);
          void         _process_service_group_member(io::data const& e);
          void         _process_service_state(io::data const& e);
          void         _process_service_status(io::data const& e);

         public:
                       stream(QString const& type,
                         QString const& host,
                         unsigned short port,
                         QString const& user,
                         QString const& password,
                         QString const& db);
                       stream(stream const& s);
                       ~stream();
          static void  initialize();
          QSharedPointer<io::data>
                       read();
          void         write(QSharedPointer<io::data> d);
        };
      }
    }
  }
}

#endif /* !CCB_SQL_STREAM_HH_ */
