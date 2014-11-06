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
#include <ctime>
#include <fstream>
#include <sstream>
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/bool_status.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/bam/monitoring_stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"

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
 *  @param[in] ext_cmd_file            External command file.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] check_replication       true to check replication status.
 */
monitoring_stream::monitoring_stream(
          QString const& db_type,
          QString const& db_host,
          unsigned short db_port,
          QString const& db_user,
          QString const& db_password,
          QString const& db_name,
          QString const& ext_cmd_file,
          unsigned int queries_per_transaction,
          bool check_replication) {
  // Process events.
  _process_out = true;

  // External command file.
  _ext_cmd_file = ext_cmd_file;

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
               << "BAM: could not connect to monitoring database '"
               << db_name << "' on host '" << db_host
               << ":" << db_port << "': " << error);
      }
    }

    // Check that replication is OK.
    if (check_replication)
      _check_replication();
    else
      logging::debug(logging::medium)
        << "BAM: NOT checking replication status of monitoring database '"
        << _db->databaseName() << "' on host '" << _db->hostName()
        << ":" << _db->port() << "'";

    // Prepare queries.
    _prepare();

    // Initial transaction.
    if (_queries_per_transaction > 1)
      _db->transaction();

    // Read configuration from DB.
    configuration::state s;
    {
      configuration::reader r(_db.get());
      r.read(s);
    }

    // Apply configuration.
    _applier.apply(s);
    _ba_mapping = s.get_ba_svc_mapping();

    // Check if we need to rebuild something.
    _rebuild();
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
monitoring_stream::~monitoring_stream() {
  // Connection ID.
  QString bam_id;
  bam_id.setNum((qulonglong)this, 16);

  {
    QMutexLocker lock(&misc::global_lock);
    // Reset statements.
    _clear_qsql();
  }

  // Remove this connection.
  QSqlDatabase::removeDatabase(bam_id);
}

/**
 *  Generate default state.
 */
void monitoring_stream::initialize() {
  multiplexing::publisher pblshr;
  _applier.visit(&pblshr);
  return ;
}

/**
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void monitoring_stream::process(bool in, bool out) {
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
void monitoring_stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
         << "BAM: attempt to read from a BAM monitoring stream (not supported)");
  return ;
}
/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void monitoring_stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty()) {
    io::property& p(tree["status"]);
    p.set_perfdata(_status);
    p.set_graphable(false);
  }
  return ;
}

/**
 *  Rebuild index and metrics cache.
 */
void monitoring_stream::update() {
  // XXX : beware of exceptions ?
  // configuration::state s;
  // {
  //   configuration::reader r(_db.get());
  //   r.read(s);
  // }
  // _applier.apply(s);
  // _ba_mapping = s.get_ba_svc_mapping();
  // Check if we need to rebuild something.
  // _rebuild();
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int monitoring_stream::write(misc::shared_ptr<io::data> const& data) {
  // Check that processing is enabled.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
           << "BAM monitoring stream is shutdown");

  if (!data.isNull()) {
    // Process service status events.
    if ((data->type()
        == io::events::data_type<io::events::neb, neb::de_service_status>::value)
        || (data->type()
            == io::events::data_type<io::events::neb, neb::de_service>::value)) {
      misc::shared_ptr<neb::service_status>
        ss(data.staticCast<neb::service_status>());
      logging::debug(logging::low)
        << "BAM: processing service status (host "
        << ss->host_id << ", service " << ss->service_id
        << ", hard state " << ss->last_hard_state << ", current state "
        << ss->current_state << ")";
      multiplexing::publisher pblshr;
      _applier.book_service().update(ss, &pblshr);
    }
    else if (data->type()
             == io::events::data_type<io::events::storage, storage::de_metric>::value) {
      misc::shared_ptr<storage::metric>
        m(data.staticCast<storage::metric>());
      logging::debug(logging::low)
        << "BAM: processing metric (id " << m->metric_id << ", time "
        << m->ctime << ", value " << m->value << ")";
      multiplexing::publisher pblshr;
      _applier.book_metric().update(m, &pblshr);
    }
    else if (data->type()
             == io::events::data_type<io::events::bam, bam::de_ba_status>::value) {
      ba_status* status(static_cast<ba_status*>(data.data()));
      logging::debug(logging::low) << "BAM: processing BA status (id "
        << status->ba_id << ", level " << status->level_nominal
        << ", acknowledgement " << status->level_acknowledgement
        << ", downtime " << status->level_downtime << ")";
      _ba_update->bindValue(":level_nominal", status->level_nominal);
      _ba_update->bindValue(
                    ":level_acknowledgement",
                    status->level_acknowledgement);
      _ba_update->bindValue(":level_downtime", status->level_downtime);
      _ba_update->bindValue(":ba_id", status->ba_id);
      _ba_update->bindValue(
        ":last_state_change",
        (((status->last_state_change == (time_t)-1)
          || (status->last_state_change == 0))
         ? QVariant(QVariant::LongLong)
         : QVariant(static_cast<qlonglong>(status->last_state_change.get_time_t()))));
      _ba_update->bindValue(":state", status->state);
      _ba_update->bindValue(":in_downtime", status->in_downtime);
      if (!_ba_update->exec())
        throw (exceptions::msg() << "BAM: could not update BA "
               << status->ba_id << ": "
               << _ba_update->lastError().text());

      if (status->state_changed) {
        std::pair<std::string, std::string>
          ba_svc_name(_ba_mapping.get_service(status->ba_id));
        if (ba_svc_name.first.empty() || ba_svc_name.second.empty()) {
          logging::error(logging::high)
            << "BAM: could not trigger check of virtual service of BA "
            << status->ba_id
            << ": host name and service description were not found";
        }
        else {
          std::ostringstream oss;
          oss << "[" << std::time(NULL) << "] PROCESS_SERVICE_CHECK_RESULT;"
              << ba_svc_name.first << ";" << ba_svc_name.second << ";"
              << status->state << ";BA " << status->ba_id << " has state "
              << status->state << " and level " << status->level_nominal
              << "|value=" << status->level_nominal;
          _write_external_command(oss.str());
        }
      }
    }
    else if (data->type()
             == io::events::data_type<io::events::bam, bam::de_bool_status>::value) {
      bool_status* status(static_cast<bool_status*>(data.data()));
      logging::debug(logging::low) << "BAM: processing boolexp status (id "
        << status->bool_id << ", state " << status->state << ")";
      _bool_exp_update->bindValue(":state", status->state);
      _bool_exp_update->bindValue(":bool_id", status->bool_id);
      if (!_bool_exp_update->exec())
        throw (exceptions::msg()
               << "BAM: could not update boolean expression "
               << status->bool_id << ": "
               << _bool_exp_update->lastError().text());
    }
    else if (data->type()
             == io::events::data_type<io::events::bam, bam::de_kpi_status>::value) {
      kpi_status* status(static_cast<kpi_status*>(data.data()));
      logging::debug(logging::low) << "BAM: processing KPI status (id "
        << status->kpi_id << ", level " << status->level_nominal_hard
        << ", acknowledgement " << status->level_acknowledgement_hard
        << ", downtime " << status->level_downtime_hard << ")";
      _kpi_update->bindValue(
                     ":level_nominal",
                     status->level_nominal_hard);
      _kpi_update->bindValue(
                     ":level_acknowledgement",
                     status->level_acknowledgement_hard);
      _kpi_update->bindValue(
                     ":level_downtime",
                     status->level_downtime_hard);
      _kpi_update->bindValue(":state", status->state_hard);
      _kpi_update->bindValue(":state_type", 1 + 1);
      _kpi_update->bindValue(":kpi_id", status->kpi_id);
      if (!_kpi_update->exec())
        throw (exceptions::msg() << "BAM: could not update KPI "
               << status->kpi_id << ": "
               << _kpi_update->lastError().text());
    }
    else if (data->type()
             == io::events::data_type<io::events::bam, bam::de_meta_service_status>::value) {
      meta_service_status* status(static_cast<meta_service_status*>(data.data()));
      logging::debug(logging::low)
        << "BAM: processing meta-service status (id "
        << status->meta_service_id << ", value " << status->value
        << ")";
      _meta_service_update->bindValue(
                              ":meta_service_id",
                              status->meta_service_id);
      _meta_service_update->bindValue(":value", status->value);
      if (!_meta_service_update->exec())
        throw (exceptions::msg()
               << "BAM: could not update meta-service "
               << status->meta_service_id << ": "
               << _meta_service_update->lastError().text());
    }
  }
  // XXX : handle transactions
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
monitoring_stream::monitoring_stream(monitoring_stream const& other)
  : io::stream(other) {
  assert(!"BAM monitoring stream is not copyable");
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] other Unused.
 *
 *  @return This object.
 */
monitoring_stream& monitoring_stream::operator=(monitoring_stream const& other) {
  (void)other;
  assert(!"BAM monitoring stream is not copyable");
  abort();
  return (*this);
}

/**
 *  Check that replication is OK.
 */
void monitoring_stream::_check_replication() {
  // Check that replication is OK.
  logging::debug(logging::medium)
    << "BAM: checking replication status of monitoring database '"
    << _db->databaseName() << "' on host '" << _db->hostName()
    << ":" << _db->port() << "'";
  QSqlQuery q(*_db);
  if (!q.exec("SHOW SLAVE STATUS"))
    logging::info(logging::medium)
      << "BAM: could not check replication status of monitoring database '"
      << _db->databaseName() << "' on host '" << _db->hostName()
      << ":" << _db->port() << "': " << q.lastError().text();
  else {
    if (!q.next())
      logging::info(logging::medium)
        << "BAM: monitoring database '" << _db->databaseName() << "' on host '"
        << _db->hostName() << ":" << _db->port()
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
                 << "BAM: replication of monitoring database '"
                 << _db->databaseName() << "' on host '"
                 << _db->hostName() << ":" << _db->port()
                 << "' is not complete: " << field
                 << "=" << q.value(i).toString());
      logging::info(logging::medium)
        << "storage: replication of monitoring database '"
        << _db->databaseName() << "' on host '" << _db->hostName()
        << ":" << _db->port() << "' is complete, connection granted";
    }
  }
  return ;
}

/**
 *  Clear QtSql objects.
 */
void monitoring_stream::_clear_qsql() {
  _ba_update.reset();
  _bool_exp_update.reset();
  _kpi_update.reset();
  _meta_service_update.reset();
  _db.reset();
  return ;
}

/**
 *  Prepare queries.
 */
void monitoring_stream::_prepare() {
  // BA status.
  {
    QString query;
    query = "UPDATE mod_bam"
            "  SET current_level=:level_nominal,"
            "      acknowledged=:level_acknowledgement,"
            "      downtime=:level_downtime,"
            "      last_state_change=:last_state_change,"
            "      in_downtime=:in_downtime,"
            "      current_status=:state"
            "  WHERE ba_id=:ba_id";
    _ba_update.reset(new QSqlQuery(*_db));
    if (!_ba_update->prepare(query))
      throw (exceptions::msg()
             << "BAM: could not prepare BA update query: "
             << _ba_update->lastError().text());
  }

  // Boolean expression status.
  {
    QString query;
    query = "UPDATE mod_bam_boolean"
            "  SET current_state=:state"
            "  WHERE boolean_id=:bool_id";
    _bool_exp_update.reset(new QSqlQuery(*_db));
    if (!_bool_exp_update->prepare(query))
      throw (exceptions::msg()
             << "BAM: could not prepare boolean expression update query: "
             << _bool_exp_update->lastError().text());
  }

  // KPI status.
  {
    QString query;
    query = "UPDATE mod_bam_kpi"
            "  SET acknowledged=:level_acknowledgement,"
            "      current_status=:state,"
            "      downtime=:level_downtime, last_level=:level_nominal,"
            "      state_type=:state_type"
            "  WHERE kpi_id=:kpi_id";
    _kpi_update.reset(new QSqlQuery(*_db));
    if (!_kpi_update->prepare(query))
      throw (exceptions::msg()
             << "BAM: could not prepare KPI update query: "
             << _kpi_update->lastError().text());
  }

  // Meta-service status.
  {
    QString query;
    query = "UPDATE meta_service"
            "  SET value=:value"
            "  WHERE meta_id=:meta_service_id";
    _meta_service_update.reset(new QSqlQuery(*_db));
    if (!_meta_service_update->prepare(query))
      throw (exceptions::msg()
             << "BAM: could not prepare meta-service update query: "
             << _meta_service_update->lastError().text());
  }

  return ;
}

/**
 *  Rebuilds BA durations/availibities from BA events.
 */
void monitoring_stream::_rebuild() {
  // Get the list of the BAs that should be rebuild.
  std::vector<unsigned int> bas_to_rebuild;
  {
    QString query = "SELECT ba_id"
                    "  FROM mod_bam"
                    "  WHERE must_be_rebuild='1'";
    QSqlQuery q = _db->exec(query);
    if (q.lastError().isValid())
      throw (exceptions::msg()
             << "BAM: could not select the list of BAs to rebuild: "
             << q.lastError().text());
    while (q.next())
      bas_to_rebuild.push_back(q.value(0).toUInt());
  }

  // Nothing to rebuild.
  if (bas_to_rebuild.empty())
    return ;

  misc::shared_ptr<rebuild> r(new rebuild);
  r->bas_to_rebuild = bas_to_rebuild;
  std::auto_ptr<io::stream> out(new multiplexing::publisher);
  out->write(r);

  // Set all the BAs to should not be rebuild.
  {
    QString query = "UPDATE mod_bam"
                    "  SET must_be_rebuild='0'";
    QSqlQuery q = _db->exec(query);
    if (q.lastError().isValid())
      throw (exceptions::msg()
             << "BAM: could not update the list of BAs to rebuild: "
             << q.lastError().text());
  }
}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void monitoring_stream::_update_status(std::string const& status) {
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}

/**
 *  Write an external command to Engine.
 *
 *  @param[in] cmd  Command to write to the external command pipe.
 */
void monitoring_stream::_write_external_command(
                          std::string const& cmd) {
  std::ofstream ofs;
  ofs.open(_ext_cmd_file.toStdString().c_str());
  if (!ofs.good()) {
    logging::error(logging::medium)
      << "BAM: could not write BA check result to command file '"
      << _ext_cmd_file << "'";
  }
  else {
    ofs.write(cmd.c_str(), cmd.size());
    if (!ofs.good())
      logging::error(logging::medium)
        << "BAM: could not write BA check result to command file '"
        << _ext_cmd_file << "'";
    else
      logging::debug(logging::medium)
        << "BAM: sent external command '" << cmd << "'";
    ofs.close();
  }
  return ;
}
