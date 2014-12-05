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

#ifndef CCB_BAM_REPORTING_STREAM_HH
#  define CCB_BAM_REPORTING_STREAM_HH

#  include <map>
#  include <memory>
#  include <vector>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QMutexLocker>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/bam/availability_thread.hh"
#  include "com/centreon/broker/bam/timeperiod_map.hh"
#  include "com/centreon/broker/bam/time/timeperiod.hh"

CCB_BEGIN()

namespace          bam {
  // Forward declarations.
  class            dimension_timeperiod;
  class            dimension_timeperiod_exception;
  class            dimension_timeperiod_exclusion;

  /**
   *  @class reporting_stream reporting_stream.hh "com/centreon/broker/bam/reporting_stream.hh"
   *  @brief bam reporting_stream.
   *
   *  Handle perfdata and insert proper informations in index_data and
   *  metrics table of a centbam DB.
   */
  class            reporting_stream : public io::stream {
  public:
                   reporting_stream(
                     QString const& db_type,
                     QString const& db_host,
                     unsigned short db_port,
                     QString const& db_user,
                     QString const& db_password,
                     QString const& db_name,
                     unsigned int queries_per_transaction,
                     bool check_replication = true);
                   ~reporting_stream();
    void           process(bool in = false, bool out = true);
    void           read(misc::shared_ptr<io::data>& d);
    void           statistics(io::properties& tree) const;
    unsigned int   write(misc::shared_ptr<io::data> const& d);

  private:
                   reporting_stream(reporting_stream const& other);
    reporting_stream&
                   operator=(reporting_stream const& other);
    void           _apply(dimension_timeperiod const& tp);
    void           _apply(dimension_timeperiod_exception const& tpe);
    void           _apply(dimension_timeperiod_exclusion const& tpe);
    void           _check_replication();
    void           _clear_qsql();
    void           _load_last_events();
    void           _load_timeperiods();
    void           _prepare();
    void           _process_ba_event(misc::shared_ptr<io::data> const& e);
    void           _process_ba_duration_event(
                     misc::shared_ptr<io::data> const& e);
    void           _process_kpi_event(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_ba(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_bv(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_ba_bv_relation(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_truncate_signal(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_kpi(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_timeperiod(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_timeperiod_exception(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_timeperiod_exclusion(misc::shared_ptr<io::data> const& e);
    void           _process_dimension_ba_timeperiod_relation(misc::shared_ptr<io::data> const& e);
    void           _process_rebuild(misc::shared_ptr<io::data> const& e);
    void           _update_status(std::string const& status);
    void           _compute_event_durations(misc::shared_ptr<ba_event> const& ev,
                                            io::stream* visitor);

    std::map<unsigned int, std::list<ba_event> >
                   _ba_event_cache;
    std::map<unsigned int, std::list<kpi_event> >
                   _kpi_event_cache;
    bool           _process_out;
    unsigned int   _queries_per_transaction;
    std::string    _status;
    mutable QMutex _statusm;
    unsigned int   _transaction_queries;
    std::auto_ptr<QSqlQuery>
                   _ba_event_insert;
    std::auto_ptr<QSqlQuery>
                   _ba_full_event_insert;
    std::auto_ptr<QSqlQuery>
                   _ba_event_update;
    std::auto_ptr<QSqlQuery>
                   _ba_event_delete;
    std::auto_ptr<QSqlQuery>
                   _ba_duration_event_insert;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_insert;
    std::auto_ptr<QSqlQuery>
                   _kpi_full_event_insert;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_update;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_delete;
    std::auto_ptr<QSqlQuery>
                   _kpi_event_link;
    std::auto_ptr<QSqlQuery>
                  _dimension_ba_insert;
    std::auto_ptr<QSqlQuery>
                  _dimension_bv_insert;
    std::auto_ptr<QSqlQuery>
                  _dimension_ba_bv_relation_insert;
    std::auto_ptr<QSqlQuery>
                  _dimension_timeperiod_insert;
    std::auto_ptr<QSqlQuery>
                  _dimension_timeperiod_exception_insert;
    std::auto_ptr<QSqlQuery>
                  _dimension_timeperiod_exclusion_insert;
    std::auto_ptr<QSqlQuery>
                  _dimension_ba_timeperiod_insert;
    std::vector<misc::shared_ptr<QSqlQuery> >
                  _dimension_truncate_tables;
    std::auto_ptr<QSqlQuery>
                  _dimension_kpi_insert;
    std::auto_ptr<QSqlDatabase>
                   _db;
    std::auto_ptr<QMutexLocker>
                  _availabilities_lock;
    std::auto_ptr<availability_thread>
                   _availabilities;

    // Timeperiods by BAs, with an option is default timeperiod.
    typedef std::multimap<unsigned int,
                          std::pair<unsigned int, bool> > timeperiod_relation_map;
    timeperiod_map _timeperiods;
    timeperiod_relation_map
                  _timeperiod_relations;
  };
}

CCB_END()

#endif // !CCB_BAM_REPORTING_STREAM_HH
