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
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include "com/centreon/broker/bam/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
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
stream::stream(
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
        _clear_qsql();
        throw (broker::exceptions::msg()
               << "BAM: could not connect to database '"
               << db_name << "' on host '" << db_host
               << ":" << db_port << "': " << _db->lastError().text());
      }
    }

    // Check that replication is OK.
    if (check_replication) {
      logging::debug(logging::medium)
        << "BAM: checking replication status of database '"
        << db_name << "' on host '" << db_host << ":" << db_port << "'";
      QSqlQuery q(*_db);
      if (!q.exec("SHOW SLAVE STATUS"))
        logging::info(logging::medium)
          << "BAM: could not check replication status of database '"
          << db_name << "' on host '" << db_host << ":" << db_port
          << "': " << q.lastError().text();
      else {
        if (!q.next())
          logging::info(logging::medium)
            << "BAM: database '" << db_name << "' on host '" << db_host
            << ":" << db_port << "' is not under replication";
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
                     << "BAM: replication of database '" << db_name
                     << "' on host '" << db_host << ":" << db_port
                     << "' is not complete: " << field
                     << "=" << q.value(i).toString());
          logging::info(logging::medium)
            << "storage: replication of database '" << db_name
            << "' on host '" << db_host << ":" << db_port
            << "' is complete, connection granted";
        }
      }
    }
    else
      logging::debug(logging::medium)
        << "BAM: NOT checking replication status of database '"
        << db_name << "' on host '" << db_host << ":" << db_port << "'";

    // Prepare queries.
    _prepare();

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
stream::~stream() {
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
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void stream::process(bool in, bool out) {
  _process_out = in || !out; // Only for immediate shutdown.
  return ;
}

/**
 *  Read from the datbase.
 *
 *  @param[out] d Cleared.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
         << "BAM: attempt to read from a BAM stream (not supported)");
  return ;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
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
void stream::update() {
  // XXX : reread configuration
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& data) {

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
stream::stream(stream const& other) : io::stream(other) {
  assert(!"BAM stream is not copyable");
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] other Unused.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& other) {
  (void)other;
  assert(!"BAM stream is not copyable");
  abort();
  return (*this);
}

/**
 *  Clear QtSql objects.
 */
void stream::_clear_qsql() {

}

/**
 *  Prepare queries.
 */
void stream::_prepare() {

}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void stream::_update_status(std::string const& status) {

}
