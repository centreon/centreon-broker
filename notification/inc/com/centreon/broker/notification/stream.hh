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

#ifndef CCB_SQL_STREAM_HH
#  define CCB_SQL_STREAM_HH

#  include <vector>
#  include <deque>
#  include <memory>
#  include <QPair>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QString>
#  include <QVector>
#  include <set>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class stream stream.hh "com/centreon/broker/notification/stream.hh"
   *  @brief Notification stream.
   *
   *  Stream events into notification database.
   */
  class          stream : public io::stream {
  public:
                 stream(
                   QString const& type,
                   QString const& host,
                   unsigned short port,
                   QString const& user,
                   QString const& password,
                   QString const& db,
                   unsigned int queries_per_transaction,
                   unsigned int cleanup_check_interval,
                   bool check_replication,
                   bool with_state_events);
                 stream(stream const& s);
                 ~stream();
    static void  initialize();
    void         process(bool in = false, bool out = false);
    void         read(misc::shared_ptr<io::data>& d);
    void         update();
    unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
    stream&      operator=(stream const& s);
    std::auto_ptr<QSqlDatabase> _db;
    bool                        _process_out;
    unsigned int                _queries_per_transaction;
    unsigned int                _transaction_queries;
    bool                        _with_state_events;
    unsigned int                _instance_timeout;
  };
}

CCB_END()

#endif // !CCB_SQL_STREAM_HH
