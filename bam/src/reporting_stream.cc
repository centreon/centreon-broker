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

#include <cassert>
#include <cstdlib>
#include <sstream>
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/bam/reporting_stream.hh"
#include "com/centreon/broker/bam/time/timezone_manager.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_type                 BAM DB type.
 *  @param[in] db_host                 BAM DB host.
 *  @param[in] db_port                 BAM DB port.
 *  @param[in] db_user                 BAM DB user.
 *  @param[in] db_password             BAM DB password.
 *  @param[in] db_name                 BAM DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] check_replication       true to check replication status.
 */
reporting_stream::reporting_stream(
          QString const& db_type,
          QString const& db_host,
          unsigned short db_port,
          QString const& db_user,
          QString const& db_password,
          QString const& db_name,
          unsigned int queries_per_transaction,
          bool check_replication) {
  // Process events.
  _process_out = true;

  // Queries per transaction.
  _queries_per_transaction = ((queries_per_transaction >= 2)
                              ? queries_per_transaction
                              : 1);
  _transaction_queries = 0;

  // BAM connection ID.
  QString bam_id;
  bam_id.setNum((qulonglong)this, 16);

  // Add database connection.
  _db.reset(
        new QSqlDatabase(QSqlDatabase::addDatabase(
                                         db_type,
                                         bam_id)));

  // Set DB parameters.
  _db->setHostName(db_host);
  _db->setPort(db_port);
  _db->setUserName(db_user);
  _db->setPassword(db_password);
  _db->setDatabaseName(db_name);

  try {
    {
      QMutexLocker lock(&misc::global_lock);
      // Open database.
      if (!_db->open()) {
        QString error(_db->lastError().text());
        _clear_qsql();
        throw (broker::exceptions::msg()
               << "BAM-BI: could not connect to reporting database '"
               << db_name << "' on host '" << db_host
               << ":" << db_port << "': " << error);
      }
    }

    // Check that replication is OK.
    if (check_replication)
      _check_replication();
    else
      logging::debug(logging::medium)
        << "BAM-BI: NOT checking replication status of reporting database '"
        << _db->databaseName() << "' on host '" << _db->hostName()
        << ":" << _db->port() << "'";

    // Prepare queries.
    _prepare();

    // Initialize timezone manager.
    time::timezone_manager::load();

    // Initialize the availabilities thread.
    _availabilities.reset(new availability_thread(
                            db_type,
                            db_host,
                            db_port,
                            db_user,
                            db_password,
                            db_name));
    // Start the availabilities thread.
    _availabilities->start();

    // Initial transaction.
    if (_queries_per_transaction > 1)
      _db->transaction();
  }
  catch (...) {
    {
      QMutexLocker lock(&misc::global_lock);
      // Delete statements.
      _clear_qsql();
    }

    // Remove this connection.
    QSqlDatabase::removeDatabase(bam_id);

    throw ;
  }
}

/**
 *  Destructor.
 */
reporting_stream::~reporting_stream() {
  // Connection ID.
  QString bam_id;
  bam_id.setNum((qulonglong)this, 16);

  {
    QMutexLocker lock(&misc::global_lock);
    // Reset statements.
    _clear_qsql();
  }

  // Terminate the availabilities thread.
  _availabilities->terminate();
  _availabilities->wait();

  // Deinitialize timezone manager.
  time::timezone_manager::unload();

  // Remove this connection.
  QSqlDatabase::removeDatabase(bam_id);
}

/**
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void reporting_stream::process(bool in, bool out) {
  _process_out = in || !out; // Only for immediate shutdown.
  return ;
}

/**
 *  Read from the datbase.
 *  Get the next available bam event.
 *
 *  @param[out] d Cleared.
 *  @param[out] d The next available bam event.
 */
void reporting_stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
         << "BAM-BI: attempt to read from a BAM reporting stream (not supported)");
  return ;
}
/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void reporting_stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty()) {
    io::property& p(tree["status"]);
    p.set_perfdata(_status);
    p.set_graphable(false);
  }
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int reporting_stream::write(misc::shared_ptr<io::data> const& data) {
  // Check that processing is enabled.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
           << "BAM reporting stream is shutdown");

  if (!data.isNull()) {
    if (data->type()
        == io::events::data_type<io::events::bam,
                                 bam::de_kpi_event>::value)
      _process_kpi_event(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_ba_event>::value)
      _process_ba_event(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_ba_duration_event>::value)
      _process_ba_duration_event(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_ba_event>::value)
      _process_dimension_ba(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_bv_event>::value)
      _process_dimension_bv(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_ba_bv_relation_event>::value)
      _process_dimension_ba_bv_relation(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_kpi_event>::value)
      _process_dimension_kpi(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_truncate_table_signal>::value)
      _process_dimension_truncate_signal(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_timeperiod>::value)
      _process_dimension_timeperiod(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_dimension_ba_timeperiod_relation>::value)
      _process_dimension_ba_timeperiod_relation(data);
    else if (data->type()
             == io::events::data_type<io::events::bam,
                                      bam::de_rebuild>::value)
    _process_rebuild(data);
  }
  // XXX : handle transaction
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] other Unused.
 */
reporting_stream::reporting_stream(reporting_stream const& other)
  : io::stream(other) {
  assert(!"BAM reporting stream is not copyable");
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] other Unused.
 *
 *  @return This object.
 */
reporting_stream& reporting_stream::operator=(reporting_stream const& other) {
  (void)other;
  assert(!"BAM reporting stream is not copyable");
  abort();
  return (*this);
}

/**
 *  Check that replication is OK.
 */
void reporting_stream::_check_replication() {
  // Check that replication is OK.
  logging::debug(logging::medium)
    << "BAM-BI: checking replication status of reporting database '"
    << _db->databaseName() << "' on host '" << _db->hostName()
    << ":" << _db->port() << "'";
  QSqlQuery q(*_db);
  if (!q.exec("SHOW SLAVE STATUS"))
    logging::info(logging::medium)
      << "BAM-BI: could not check replication status of reporting database '"
      << _db->databaseName() << "' on host '" << _db->hostName()
      << ":" << _db->port() << "': " << q.lastError().text();
  else {
    if (!q.next())
      logging::info(logging::medium)
        << "BAM-BI: reorting database '" << _db->databaseName()
        << "' on host '" << _db->hostName() << ":" << _db->port()
        << "' is not under replication";
    else {
      QSqlRecord record(q.record());
      unsigned int i(0);
      for (QString field(record.fieldName(i));
           !field.isEmpty();
           field = record.fieldName(++i))
        if (((field == "Slave_IO_Running")
             && (q.value(i).toString() != "Yes"))
            || ((field == "Slave_SQL_Running")
                && (q.value(i).toString() != "Yes"))
            || ((field == "Seconds_Behind_Master")
                && (q.value(i).toInt() != 0)))
          throw (broker::exceptions::msg()
                 << "BAM-BI: replication of reporting database '"
                 << _db->databaseName() << "' on host '"
                 << _db->hostName() << ":" << _db->port()
                 << "' is not complete: " << field
                 << "=" << q.value(i).toString());
      logging::info(logging::medium)
        << "storage: replication of reporting database '"
        << _db->databaseName() << "' on host '" << _db->hostName()
        << ":" << _db->port() << "' is complete, connection granted";
    }
  }
  return ;
}

/**
 *  Clear QtSql objects.
 */
void reporting_stream::_clear_qsql() {
  _ba_event_insert.reset();
  _ba_event_update.reset();
  _ba_event_delete.reset();
  _ba_duration_event_insert.reset();
  _kpi_event_insert.reset();
  _kpi_event_update.reset();
  _kpi_event_delete.reset();
  _kpi_event_link.reset();
  _dimension_ba_bv_relation_insert.reset();
  _dimension_ba_insert.reset();
  _dimension_bv_insert.reset();
  _dimension_truncate_tables.clear();
  _dimension_kpi_insert.reset();
  _db.reset();
  return ;
}

/**
 *  Prepare queries.
 */
void reporting_stream::_prepare() {
  // BA event insertion.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_ba_events (ba_id, "
            "            first_level, start_time, status, in_downtime)"
            "  VALUES (:ba_id, :first_level,"
            "          :start_time, :status, :in_downtime)";
    _ba_event_insert.reset(new QSqlQuery(*_db));
    if (!_ba_event_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare BA event insertion query: "
             << _ba_event_insert->lastError().text());
  }

  // BA event update.
  {
    QString query;
    query = "UPDATE mod_bam_reporting_ba_events"
            "  SET end_time=:end_time"
            "  WHERE ba_id=:ba_id AND start_time=:start_time";
    _ba_event_update.reset(new QSqlQuery(*_db));
    if (!_ba_event_update->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare BA event update query: "
             << _ba_event_update->lastError().text());
  }

  // BA event deletion.
  {
    QString query;
    query = "DELETE FROM mod_bam_reporting_ba_events"
            "  WHERE ba_id=:ba_id AND start_time=:start_time";
    _ba_event_delete.reset(new QSqlQuery(*_db));
    if (!_ba_event_delete->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare BA event deletion query: "
             << _ba_event_delete->lastError().text());
  }

  // BA duration event insert.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_ba_events_durations ("
             "                ba_event_id, start_time, "
             "                end_time, duration, sla_duration, timeperiod_id, "
             "                timeperiod_is_default)"
             "  SELECT b.ba_event_id, :start_time, :end_time, :duration, "
             "         :sla_duration, :timeperiod_id, :timeperiod_is_default"
             "  FROM mod_bam_reporting_ba_events AS b"
             "  WHERE b.ba_id=:ba_id AND b.start_time=:real_start_time";
    _ba_duration_event_insert.reset(new QSqlQuery(*_db));
    if (!_ba_duration_event_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare BA duration event insert query: "
             << _ba_duration_event_insert->lastError().text());
  }

  // KPI event insertion.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_kpi_events (kpi_id,"
            "            start_time, status, in_downtime, impact_level,"
            "            first_output, first_perfdata)"
            "  VALUES (:kpi_id, :start_time, :status, :in_downtime, "
            "         :impact_level, :output, :perfdata)";
    _kpi_event_insert.reset(new QSqlQuery(*_db));
    if (!_kpi_event_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare KPI event insertion query: "
             << _kpi_event_insert->lastError().text());
  }

  // KPI event update.
  {
    QString query;
    query = "UPDATE mod_bam_reporting_kpi_events"
            "  SET end_time=:end_time"
            "  WHERE kpi_id=:kpi_id AND start_time=:start_time";
    _kpi_event_update.reset(new QSqlQuery(*_db));
    if (!_kpi_event_update->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare KPI event update query: "
             << _kpi_event_update->lastError().text());
  }

  // KPI event deletion.
  {
    QString query;
    query = "DELETE FROM mod_bam_reporting_kpi_events"
            "  WHERE kpi_id=:kpi_id AND start_time=:start_time";
    _kpi_event_delete.reset(new QSqlQuery(*_db));
    if (!_kpi_event_delete->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare KPI event deletion query: "
             << _kpi_event_delete->lastError().text());
  }

  // KPI event link to BA event.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_relations_ba_kpi_events"
            "           (ba_event_id, kpi_event_id)"
            "  SELECT be.ba_event_id, ke.kpi_event_id"
            "    FROM mod_bam_reporting_kpi_events AS ke"
            "    INNER JOIN mod_bam_reporting_ba_events AS be"
            "    ON ((ke.start_time >= be.start_time)"
            "       AND (be.end_time IS NULL OR ke.start_time < be.end_time))"
            "    WHERE ke.kpi_id=:kpi_id AND ke.start_time=:start_time";
    _kpi_event_link.reset(new QSqlQuery(*_db));
    if (!_kpi_event_link->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare link query of BA and KPI events: "
             << _kpi_event_link->lastError().text());
  }

  // Dimension BA insertion.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
            "                sla_month_percent_1, sla_month_percent_2,"
            "                sla_month_duration_1, sla_month_duration_2)"
            " VALUES (:ba_id, :ba_name, :ba_description, :sla_month_percent_1,"
            "         :sla_month_percent_2, :sla_month_duration_1,"
            "         :sla_month_duration_2)";
    _dimension_ba_insert.reset(new QSqlQuery(*_db));
    if (!_dimension_ba_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare the insertion of BA dimensions: "
             << _dimension_ba_insert->lastError().text());
  }

  // Dimension BV insertion.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_bv (bv_id, bv_name, bv_description)"
            "  VALUES (:bv_id, :bv_name, :bv_description)";
    _dimension_bv_insert.reset(new QSqlQuery(*_db));
    if (!_dimension_bv_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare the insertion of BV dimensions: "
             << _dimension_bv_insert->lastError().text());
  }

  // Dimension BA BV relations insertion.
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_relations_ba_bv (ba_id, bv_id)"
            "  VALUES (:ba_id, :bv_id)";
    _dimension_ba_bv_relation_insert.reset(new QSqlQuery(*_db));
    if (!_dimension_ba_bv_relation_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare the insertion of BA BV"
                "relation dimension: "
             << _dimension_ba_bv_relation_insert->lastError().text());
  }

  // Dimension truncate tables.
  {
    _dimension_truncate_tables.clear();
    QString query;
    query = "DELETE FROM mod_bam_reporting_kpi";
    _dimension_truncate_tables.push_back(
          misc::shared_ptr<QSqlQuery>(new QSqlQuery(*_db)));
    if (!_dimension_truncate_tables.back()->prepare(query))
      throw (exceptions::msg()
            << "BAM-BI: could not prepare the truncate of table kpi");
    query = "DELETE FROM mod_bam_reporting_relations_ba_bv";
    _dimension_truncate_tables.push_back(
          misc::shared_ptr<QSqlQuery>(new QSqlQuery(*_db)));
    if (!_dimension_truncate_tables.back()->prepare(query))
      throw (exceptions::msg()
            << "BAM-BI: could not prepare the truncate of table relations_ba_bv");
    query = "DELETE FROM mod_bam_reporting_ba";
    _dimension_truncate_tables.push_back(
          misc::shared_ptr<QSqlQuery>(new QSqlQuery(*_db)));
    if (!_dimension_truncate_tables.back()->prepare(query))
      throw (exceptions::msg()
            << "BAM-BI: could not prepare the truncate of table ba");
    query = "DELETE FROM mod_bam_reporting_bv";
    _dimension_truncate_tables.push_back(
          misc::shared_ptr<QSqlQuery>(new QSqlQuery(*_db)));
    if (!_dimension_truncate_tables.back()->prepare(query))
      throw (exceptions::msg()
            << "BAM-BI: could not prepare the truncate of table bv");
  }

  // Dimension KPI insertion
  {
    QString query;
    query = "INSERT INTO mod_bam_reporting_kpi (kpi_id, kpi_name,"
            "            ba_id, ba_name, host_id, host_name,"
            "            service_id, service_description, kpi_ba_id,"
            "            kpi_ba_name, meta_service_id, meta_service_name,"
            "            impact_warning, impact_critical, impact_unknown,"
            "            boolean_id, boolean_name)"
            "  VALUES (:kpi_id, :kpi_name, :ba_id, :ba_name, :host_id,"
            "          :host_name, :service_id, :service_description,"
            "          :kpi_ba_id, :kpi_ba_name, :meta_service_id,"
            "          :meta_service_name, :impact_warning, :impact_critical,"
            "          :impact_unknown, :boolean_id, :boolean_name)";
    _dimension_kpi_insert.reset(new QSqlQuery(*_db));
    if (!_dimension_kpi_insert->prepare(query))
      throw (exceptions::msg()
             << "BAM-BI: could not prepare the insertion of KPI dimensions: "
             << _dimension_kpi_insert->lastError().text());
  }

  return ;
}

/**
 *  Process a ba event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_ba_event(misc::shared_ptr<io::data> const& e) {
  bam::ba_event const& be = e.ref_as<bam::ba_event const>();
  logging::debug(logging::low) << "BAM-BI: processing event of BA "
    << be.ba_id << " (start time " << be.start_time << ", end time "
    << be.end_time << ", status " << be.status << ", in downtime "
    << be.in_downtime << ")";
  // Ephemeral event.
  if (be.start_time == be.end_time) {
    _ba_event_delete->bindValue(":ba_id", be.ba_id);
    _ba_event_delete->bindValue(
      ":start_time",
      static_cast<qlonglong>(be.start_time.get_time_t()));
    if (!_ba_event_delete->exec())
      throw (exceptions::msg()
             << "BAM-BI: could not delete ephemeral event of BA "
             << be.ba_id << " at second " << be.start_time);
    std::map<unsigned int, std::list<ba_event> >::iterator
      it(_ba_event_cache.find(be.ba_id));
    if ((it != _ba_event_cache.end())
        && !it->second.empty()
        && (be.start_time == it->second.front().start_time)) {
      it->second.pop_front();
      if (it->second.empty())
        _ba_event_cache.erase(it);
    }
  }
  // End of event.
  else if ((be.end_time != 0) && (be.end_time != (time_t)-1)) {
    std::map<unsigned int, std::list<ba_event> >::iterator
      it(_ba_event_cache.find(be.ba_id));
    if (it == _ba_event_cache.end())
      _ba_event_cache[be.ba_id].push_front(be);
    else if (it->second.empty())
      it->second.push_front(be);
    else
      it->second.front().end_time = be.end_time;
    _ba_event_update->bindValue(":ba_id", be.ba_id);
    _ba_event_update->bindValue(
      ":start_time",
      static_cast<qlonglong>(it->second.front().start_time.get_time_t()));
    _ba_event_update->bindValue(
      ":end_time",
      static_cast<qlonglong>(it->second.front().end_time.get_time_t()));
    if (!_ba_event_update->exec())
      throw (exceptions::msg() << "BAM-BI: could not close event of BA "
             << be.ba_id << " starting at " << be.start_time
             << " and ending at " << be.end_time);

    // Compute the associated event durations.
    _compute_event_durations(e.staticCast<bam::ba_event>(), this);
  }
  // Start of event.
  else {
    std::map<unsigned int, std::list<ba_event> >::iterator
      it(_ba_event_cache.find(be.ba_id));
    // Reopen event.
    if ((it != _ba_event_cache.end())
        && !it->second.empty()
        && (it->second.front().end_time == be.start_time)
        && (it->second.front().in_downtime == be.in_downtime)
        && (it->second.front().status == be.status)) {
      _ba_event_update->bindValue(":ba_id", be.ba_id);
      _ba_event_update->bindValue(
        ":start_time",
        static_cast<qlonglong>(it->second.front().start_time.get_time_t()));
      _ba_event_update->bindValue(
        ":end_time",
        QVariant(QVariant::LongLong));
      if (!_ba_event_update->exec())
        throw (exceptions::msg()
               << "BAM-BI: could not reopen event of BA "
               << be.ba_id << " starting at " << be.start_time << ": "
               << _ba_event_update->lastError().text());
      it->second.front().end_time = (time_t)-1;
    }
    // Open new event.
    else {
      _ba_event_insert->bindValue(":ba_id", be.ba_id);
      _ba_event_insert->bindValue(":first_level", be.first_level);
      _ba_event_insert->bindValue(
        ":start_time",
        static_cast<qlonglong>(be.start_time.get_time_t()));
      _ba_event_insert->bindValue(":status", be.status);
      _ba_event_insert->bindValue(":in_downtime", be.in_downtime);
      if (!_ba_event_insert->exec())
        throw (exceptions::msg()
               << "BAM-BI: could not insert event of BA "
               << be.ba_id << " starting at " << be.start_time);
      std::list<ba_event>& event_list(_ba_event_cache[be.ba_id]);
      event_list.push_front(be);
      if (event_list.size() > 2)
        event_list.pop_back();
    }
  }
  return ;
}

/**
 *  Process a ba duration event and write it to the db.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_ba_duration_event(
    misc::shared_ptr<io::data> const& e) {
  bam::ba_duration_event const& bde = e.ref_as<bam::ba_duration_event const>();
  logging::debug(logging::low) << "BAM-BI: processing BA duration event of BA "
    << bde.ba_id << " (start time " << bde.start_time << ", end time "
    << bde.end_time << ", duration " << bde.duration << ", sla duration "
    << bde.sla_duration << ")";
  _ba_duration_event_insert->bindValue(":ba_id", bde.ba_id);
  _ba_duration_event_insert->bindValue(
    ":real_start_time",
    static_cast<qlonglong>(bde.real_start_time.get_time_t()));
  _ba_duration_event_insert->bindValue(
    ":end_time",
    static_cast<qlonglong>(bde.end_time.get_time_t()));
  _ba_duration_event_insert->bindValue(
    ":start_time",
    static_cast<qlonglong>(bde.start_time.get_time_t()));
  _ba_duration_event_insert->bindValue(":duration",
                                       bde.duration);
  _ba_duration_event_insert->bindValue(":sla_duration",
                                       bde.sla_duration);
  _ba_duration_event_insert->bindValue(":timeperiod_id",
                                       bde.timeperiod_id);
  _ba_duration_event_insert->bindValue(
    ":timeperiod_is_default",
    bde.timeperiod_is_default);
  if (!_ba_duration_event_insert->exec())
    throw (exceptions::msg() << "BAM-BI: could not insert duration event of BA "
           << bde.ba_id << " starting at " << bde.start_time << ": " << _ba_duration_event_insert->lastError().text());
  return ;
}

/**
 *  Process a kpi event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_kpi_event(
    misc::shared_ptr<io::data> const& e) {
  bam::kpi_event const& ke = e.ref_as<bam::kpi_event const>();
  logging::debug(logging::low) << "BAM-BI: processing event of KPI "
    << ke.kpi_id << " (start time " << ke.start_time << ", end time "
    << ke.end_time << ", state " << ke.status << ", in downtime "
    << ke.in_downtime << ")";
  // Ephemeral event.
  if (ke.start_time == ke.end_time) {
    _kpi_event_delete->bindValue(":kpi_id", ke.kpi_id);
    _kpi_event_delete->bindValue(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    if (!_kpi_event_delete->exec())
      throw (exceptions::msg()
             << "BAM-BI: could not delete ephemeral event of KPI "
             << ke.kpi_id << " at second " << ke.start_time);
    std::map<unsigned int, std::list<kpi_event> >::iterator
      it(_kpi_event_cache.find(ke.kpi_id));
    if ((it != _kpi_event_cache.end())
        && !it->second.empty()
        && (ke.start_time == it->second.front().start_time)) {
      it->second.pop_front();
      if (it->second.empty())
        _kpi_event_cache.erase(it);
    }
  }
  // End of event.
  if ((ke.end_time != 0) && (ke.end_time != (time_t)-1)) {
    std::map<unsigned int, std::list<kpi_event> >::iterator
      it(_kpi_event_cache.find(ke.kpi_id));
    if (it == _kpi_event_cache.end())
      _kpi_event_cache[ke.kpi_id].push_front(ke);
    else if (it->second.empty())
      it->second.push_front(ke);
    else
      it->second.front().end_time = ke.end_time;
    _kpi_event_update->bindValue(":kpi_id", ke.kpi_id);
    _kpi_event_update->bindValue(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    _kpi_event_update->bindValue(
      ":end_time",
      static_cast<qlonglong>(ke.end_time.get_time_t()));
    if (!_kpi_event_update->exec())
      throw (exceptions::msg() << "BAM-BI: could not close event of KPI "
             << ke.kpi_id << " starting at " << ke.start_time
             << " and ending at " << ke.end_time << ": "
             << _kpi_event_update->lastError().text());

    _kpi_event_link->bindValue(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    _kpi_event_link->bindValue(":kpi_id", ke.kpi_id);
    if (!_kpi_event_link->exec())
      throw (exceptions::msg()
             << "BAM-BI: could not create link from event of KPI "
             << ke.kpi_id << " starting at " << ke.start_time
             << " to its associated BA event: "
             << _kpi_event_link->lastError().text());
  }
  // Start of event.
  else {
    std::map<unsigned int, std::list<kpi_event> >::iterator
      it(_kpi_event_cache.find(ke.kpi_id));
    // Reopen event.
    if ((it != _kpi_event_cache.end())
        && !it->second.empty()
        && (it->second.front().end_time == ke.start_time)
        && (it->second.front().in_downtime == ke.in_downtime)
        && (it->second.front().status == ke.status)) {
      _kpi_event_update->bindValue(":kpi_id", ke.kpi_id);
      _kpi_event_update->bindValue(
        ":start_time",
        static_cast<qlonglong>(it->second.front().start_time.get_time_t()));
      _kpi_event_update->bindValue(
        ":end_time",
        QVariant(QVariant::LongLong));
      if (!_kpi_event_update->exec())
        throw (exceptions::msg()
               << "BAM-BI: could not reopen event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time << ": "
               << _kpi_event_update->lastError().text());
      it->second.front().end_time = (time_t)-1;
    }
    // Open new event.
    else {
      _kpi_event_insert->bindValue(":kpi_id", ke.kpi_id);
      _kpi_event_insert->bindValue(
        ":start_time",
        static_cast<qlonglong>(ke.start_time.get_time_t()));
      _kpi_event_insert->bindValue(":status", ke.status);
      _kpi_event_insert->bindValue(":in_downtime", ke.in_downtime);
      _kpi_event_insert->bindValue(":impact_level", ke.impact_level);
      _kpi_event_insert->bindValue(":output", ke.output);
      _kpi_event_insert->bindValue(":perfdata", ke.perfdata);
      if (!_kpi_event_insert->exec())
        throw (exceptions::msg()
               << "BAM-BI: could not insert event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time << ": "
               << _kpi_event_insert->lastError().text());
      std::list<kpi_event>& event_list(_kpi_event_cache[ke.kpi_id]);
      event_list.push_front(ke);
      if (event_list.size() > 2)
        event_list.pop_back();
    }
  }
  return ;
}

/**
 *  Process a dimension ba and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_ba(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_ba_event const& dba = e.ref_as<bam::dimension_ba_event const>();
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of BA "
    << dba.ba_id << " ('" << dba.ba_description << "')";
  _dimension_ba_insert->bindValue(":ba_id", dba.ba_id);
  _dimension_ba_insert->bindValue(":ba_name", dba.ba_name);
  _dimension_ba_insert->bindValue(
                          ":ba_description",
                          dba.ba_description);
  _dimension_ba_insert->bindValue(
                          ":sla_month_percent_1",
                          dba.sla_month_percent_1);
  _dimension_ba_insert->bindValue(
                          ":sla_month_percent_2",
                          dba.sla_month_percent_2);
  _dimension_ba_insert->bindValue(
                          ":sla_month_duration_1",
                          dba.sla_duration_1);
  _dimension_ba_insert->bindValue(
                          ":sla_month_duration_2"
                          , dba.sla_duration_2);
  if (!_dimension_ba_insert->exec())
    throw (exceptions::msg() << "BAM-BI: could not insert BA "
           << dba.ba_id << " :"
           << _dimension_ba_insert->lastError().text());
}

/**
 *  Process a dimension bv and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_bv(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_bv_event const& dbv =
      e.ref_as<bam::dimension_bv_event const>();
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of BV "
    << dbv.bv_id << " ('" << dbv.bv_name << "')";
  _dimension_bv_insert->bindValue(":bv_id", dbv.bv_id);
  _dimension_bv_insert->bindValue(":bv_name", dbv.bv_name);
  _dimension_bv_insert->bindValue(
                          ":bv_description",
                          dbv.bv_description);
  if (!_dimension_bv_insert->exec())
    throw (exceptions::msg() << "BAM-BI: could not insert BV "
           << dbv.bv_id << " :"
           << _dimension_bv_insert->lastError().text());
}

/**
 *  Process a dimension ba bv relation and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_ba_bv_relation(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_ba_bv_relation_event const& dbabv =
    e.ref_as<bam::dimension_ba_bv_relation_event const>();
  logging::debug(logging::low)
    << "BAM-BI: processing relation between BA "
    << dbabv.ba_id << " and BV " << dbabv.bv_id;
  _dimension_ba_bv_relation_insert->bindValue(":ba_id", dbabv.ba_id);
  _dimension_ba_bv_relation_insert->bindValue(":bv_id", dbabv.bv_id);
  if (!_dimension_ba_bv_relation_insert->exec())
    throw (exceptions::msg() << "BAM-BI: could not insert dimension of "
                                "BA-BV relation "
           << dbabv.ba_id << "- "<< dbabv.bv_id << " :"
           << _dimension_ba_bv_relation_insert->lastError().text());
}

/**
 *  Process a dimension truncate signal and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_truncate_signal(
    misc::shared_ptr<io::data> const& e) {
  (void)e;
  logging::debug(logging::low)
    << "BAM-BI: processing table truncation signal";
  for (std::vector<misc::shared_ptr<QSqlQuery> >::iterator
         it(_dimension_truncate_tables.begin()),
         end(_dimension_truncate_tables.end());
       it != end;
       ++it)
    if (!(*it)->exec())
      throw (exceptions::msg()
             << "BAM-BI: could not truncate dimension tables: "
             << (*it)->lastError().text());
  _timeperiods.clear();
  _timeperiod_relations.clear();
}

/**
 *  Process a dimension KPI and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_kpi(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_kpi_event const& dk =
      e.ref_as<bam::dimension_kpi_event const>();
  QString kpi_name;
  if (!dk.service_description.isEmpty())
    kpi_name = dk.service_description;
  else if (!dk.kpi_ba_name.isEmpty())
    kpi_name = dk.kpi_ba_name;
  else if (!dk.boolean_name.isEmpty())
    kpi_name = dk.boolean_name;
  else if (!dk.meta_service_name.isEmpty())
    kpi_name = dk.meta_service_name;
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of KPI "
    << dk.kpi_id << " ('" << kpi_name << "')";
  _dimension_kpi_insert->bindValue(":kpi_id", dk.kpi_id);
  _dimension_kpi_insert->bindValue(":kpi_name", kpi_name);
  _dimension_kpi_insert->bindValue(":ba_id", dk.ba_id);
  _dimension_kpi_insert->bindValue(":ba_name", dk.ba_name);
  _dimension_kpi_insert->bindValue(":host_id", dk.host_id);
  _dimension_kpi_insert->bindValue(":host_name", dk.host_name);
  _dimension_kpi_insert->bindValue(":service_id", dk.service_id);
  _dimension_kpi_insert->bindValue(
                           ":service_description",
                           dk.service_description);
  _dimension_kpi_insert->bindValue(":kpi_ba_id", dk.kpi_ba_id != 0 ?
                                                    dk.kpi_ba_id :
                                                    QVariant(QVariant::UInt));
  _dimension_kpi_insert->bindValue(":kpi_ba_name", dk.kpi_ba_name);
  _dimension_kpi_insert->bindValue(":meta_service_id", dk.meta_service_id);
  _dimension_kpi_insert->bindValue(
                           ":meta_service_name",
                           dk.meta_service_name);
  _dimension_kpi_insert->bindValue(":impact_warning", dk.impact_warning);
  _dimension_kpi_insert->bindValue(":impact_critical", dk.impact_critical);
  _dimension_kpi_insert->bindValue(":impact_unknown", dk.impact_unknown);
  _dimension_kpi_insert->bindValue(":boolean_id", dk.boolean_id);
  _dimension_kpi_insert->bindValue(":boolean_name", dk.boolean_name);
  if (!_dimension_kpi_insert->exec())
    throw (exceptions::msg() << "BAM-BI: could not insert dimension of KPI "
           << dk.kpi_id << " :"
           << _dimension_kpi_insert->lastError().text());
}

/**
 *  Process a dimension timeperiod and store it in the timeperiod caches.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod(
        misc::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod const& tp =
      e.ref_as<bam::dimension_timeperiod const>();
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of timeperiod "
    << tp.timeperiod->get_id();
  _timeperiods[tp.timeperiod->get_id()] = tp.timeperiod;
}

/**
 *  Process a dimension ba timeperiod relation and store it in
 *  a relation cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_ba_timeperiod_relation(
        misc::shared_ptr<io::data> const& e) {
  bam::dimension_ba_timeperiod_relation const& r =
     e.ref_as<bam::dimension_ba_timeperiod_relation const>();
  logging::debug(logging::low)
    << "BAM-BI: processing relation of BA " << r.ba_id
    << " to timeperiod " << r.timeperiod_id;
  _timeperiod_relations.insert(std::make_pair(r.ba_id,
                                              std::make_pair(r.timeperiod_id,
                                                             r.is_default)));
}

/**
 *  @brief Compute and write the duration events associated with a ba event.
 *
 *  The event durations are computed from the associated timeperiods of the BA.
 *
 *  @param[in] ev       The ba_event generating the durations.
 *  @param[in] visitor  A visitor stream.
 */
void reporting_stream::_compute_event_durations(
                         misc::shared_ptr<ba_event> const& ev,
                         io::stream* visitor) {
  if (ev.isNull() || !visitor)
    return ;

  logging::debug(logging::low)
    << "BAM-BI: computing event durations for BA " << ev->ba_id;

  // Find the timeperiods associated with this ba.
  std::pair<timeperiod_relation_map::const_iterator,
            timeperiod_relation_map::const_iterator> found
      = _timeperiod_relations.equal_range(ev->ba_id);

  if (found.first == found.second)
    return ;

  for (; found.first != found.second; ++found.first) {
    unsigned int tp_id = found.first->second.first;
    timeperiod_map::const_iterator tp_found = _timeperiods.find(tp_id);
    if (tp_found == _timeperiods.end()) {
      throw exceptions::msg() << "BAM-BI: could not find the timeperiod "
                              << tp_id << " in cache";
    }
    time::timeperiod::ptr tp = tp_found->second;
    bool is_default = found.first->second.second;

    misc::shared_ptr<ba_duration_event> dur_ev(new ba_duration_event);
    dur_ev->ba_id = ev->ba_id;
    dur_ev->real_start_time = ev->start_time;
    dur_ev->start_time = tp->get_next_valid(ev->start_time);
    dur_ev->end_time = ev->end_time;
    dur_ev->duration = dur_ev->end_time - dur_ev->start_time;
    dur_ev->sla_duration = tp->duration_intersect(dur_ev->start_time,
                                                  dur_ev->end_time);
    dur_ev->timeperiod_id = tp->get_id();
    dur_ev->timeperiod_is_default = is_default;
    visitor->write(dur_ev.staticCast<io::data>());
  }
}

/**
 *  Process a rebuild signal: Delete the obsolete data in the db and rebuild
 *  ba duration events.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_rebuild(misc::shared_ptr<io::data> const& e) {
  rebuild const& r = e.ref_as<rebuild const>();
  if (r.bas_to_rebuild.isEmpty())
    return ;
  logging::debug(logging::low)
    << "BAM-BI: processing rebuild signal";

  // Delete obsolete ba events durations.
  {
    QString query;
    query.append("DELETE mod_bam_reporting_ba_events_durations"
                 "  FROM mod_bam_reporting_ba_events_durations"
                 "    INNER JOIN mod_bam_reporting_ba_events as b"
                 "  WHERE b.ba_id IN (");
    query.append(r.bas_to_rebuild);
    query.append(")");
    QSqlQuery q(*_db);
    if (!q.exec(query))
      throw (exceptions::msg()
             << "BAM-BI: could not delete BA durations "
             << r.bas_to_rebuild << " :" << q.lastError().text());
  }

  // Get the ba events.
  std::vector<misc::shared_ptr<ba_event> > ba_events;
  {
    QString query = "SELECT ba_id, start_time, end_time, "
                    "status, in_downtime boolean"
                    "  FROM mod_bam_reporting_ba_events"
                    "  WHERE end_time != 0"
                    "    AND ba_id IN (";
    query.append(r.bas_to_rebuild);
    query.append(")");
    QSqlQuery q(*_db);
    if (!q.exec(query))
      throw (exceptions::msg()
             << "BAM-BI: could not get BA events of "
             << r.bas_to_rebuild << " :" << q.lastError().text());
    while (q.next()) {
      misc::shared_ptr<ba_event> baev(new ba_event);
      baev->ba_id = q.value(0).toInt();
      baev->start_time = q.value(1).toInt();
      baev->end_time = q.value(2).toInt();
      baev->status = q.value(3).toInt();
      baev->in_downtime = q.value(4).toBool();
      ba_events.push_back(baev);
      logging::debug(logging::low)
        << "BAM-BI: got ba events of " << baev->ba_id;
    }
  }

  // Generate new ba events durations for each ba events.
  {
    for (std::vector<misc::shared_ptr<ba_event> >::const_iterator
           it(ba_events.begin()),
           end(ba_events.end());
         it != end;
         ++it)
      _compute_event_durations(*it, this);
  }

  // Ask for the availabilities thread to recompute the availabilities.
  _availabilities->rebuild_availabilities();
}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void reporting_stream::_update_status(std::string const& status) {
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}
