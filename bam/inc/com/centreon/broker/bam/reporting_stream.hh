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
#  include <QMutexLocker>
#  include "com/centreon/broker/database.hh"
#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/bam/availability_thread.hh"
#  include "com/centreon/broker/bam/timeperiod_map.hh"
#  include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

// Forward declaration.
class              database_config;

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
                   reporting_stream(database_config const& db_cfg);
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
    void           _close_inconsistent_events(
                     char const* event_type,
                     char const* table,
                     char const* id);
    void           _load_last_events();
    void           _load_timeperiods();
    void           _prepare();
    void           _process_ba_event(misc::shared_ptr<io::data> const& e);
    void           _process_ba_duration_event(
                     misc::shared_ptr<io::data> const& e);
    void           _process_kpi_event(misc::shared_ptr<io::data> const& e);
    void           _process_dimension(misc::shared_ptr<io::data> const& e);
    void           _dimension_dispatch(misc::shared_ptr<io::data> const& e);
    misc::shared_ptr<io::data>
                   _dimension_copy(misc::shared_ptr<io::data> const& e);
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
    unsigned int   _pending_events;
    bool           _process_out;
    unsigned int   _queries_per_transaction;
    std::string    _status;
    mutable QMutex _statusm;
    unsigned int   _transaction_queries;
    database       _db;
    database_query _ba_event_insert;
    database_query _ba_full_event_insert;
    database_query _ba_event_update;
    database_query _ba_event_delete;
    database_query _ba_duration_event_insert;
    database_query _kpi_event_insert;
    database_query _kpi_full_event_insert;
    database_query _kpi_event_update;
    database_query _kpi_event_delete;
    database_query _kpi_event_link;
    database_query _dimension_ba_insert;
    database_query _dimension_bv_insert;
    database_query _dimension_ba_bv_relation_insert;
    database_query _dimension_timeperiod_insert;
    database_query _dimension_timeperiod_exception_insert;
    database_query _dimension_timeperiod_exclusion_insert;
    database_query _dimension_ba_timeperiod_insert;
    database_query _dimension_kpi_insert;
    std::vector<misc::shared_ptr<database_query> >
                   _dimension_truncate_tables;
    std::auto_ptr<availability_thread>
                   _availabilities;

    // Timeperiods by BAs, with an option is default timeperiod.
    timeperiod_map _timeperiods;

    std::vector<misc::shared_ptr<io::data> >
                   _dimension_data_cache;
  };
}

CCB_END()

#endif // !CCB_BAM_REPORTING_STREAM_HH
