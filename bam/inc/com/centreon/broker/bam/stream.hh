/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_STREAM_HH
#  define CCB_BAM_STREAM_HH

#  include <memory>
#  include <QList>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QMap>
#  include "com/centreon/broker/bam/configuration/applier/state.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class stream stream.hh "com/centreon/broker/bam/stream.hh"
   *  @brief bam stream.
   *
   *  Handle perfdata and insert proper informations in index_data and
   *  metrics table of a centbam DB.
   */
  class            stream : public io::stream {
  public:
                   stream(
                     QString const& db_type,
                     QString const& db_host,
                     unsigned short db_port,
                     QString const& db_user,
                     QString const& db_password,
                     QString const& db_name,
                     unsigned int queries_per_transaction,
                     bool check_replication = true);
                   ~stream();
    void           initialize();
    void           process(bool in = false, bool out = true);
    void           read(misc::shared_ptr<io::data>& d);
    void           statistics(io::properties& tree) const;
    void           update();
    unsigned int   write(misc::shared_ptr<io::data> const& d);

  private:
                   stream(stream const& other);
    stream&        operator=(stream const& other);
    void           _check_replication();
    void           _clear_qsql();
    void           _prepare();
    void           _process_ba_event(misc::shared_ptr<io::data> const& e);
    void           _process_kpi_event(misc::shared_ptr<io::data> const& e);
    void           _update_status(std::string const& status);

    configuration::applier::state
                   _applier;
    bool           _process_out;
    unsigned int   _queries_per_transaction;
    std::string    _status;
    mutable QMutex _statusm;
    unsigned int   _transaction_queries;
    std::auto_ptr<QSqlQuery>
                   _ba_update;
    std::auto_ptr<QSqlQuery>
                   _bool_exp_update;
    std::auto_ptr<QSqlQuery>
                   _kpi_update;
    std::auto_ptr<QSqlQuery>
                   _meta_service_update;
    std::auto_ptr<QSqlQuery>
                   _ba_event_insert;
    std::auto_ptr<QSqlQuery>
                   _ba_event_update;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_insert;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_update;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_link;
    std::auto_ptr<QSqlDatabase>
                   _db;
  };
}

CCB_END()

#endif // !CCB_BAM_STREAM_HH
