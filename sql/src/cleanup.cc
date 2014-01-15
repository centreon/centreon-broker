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

#include <ctime>
#include <QSqlError>
#include <QSqlQuery>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/sql/cleanup.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
cleanup::cleanup()
  : _interval(600),
    _should_exit(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
cleanup::cleanup(cleanup const& right) : QThread() {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
cleanup::~cleanup() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
cleanup& cleanup::operator=(cleanup const& right) {
  if (this != &right) {
    if (_db.isOpen())
      _db.close();
    if (!_db.connectionName().isEmpty())
      QSqlDatabase::removeDatabase(_db.connectionName());
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Set the exit flag.
 */
void cleanup::exit() throw () {
  _should_exit = true;
  return ;
}

/**
 *  Get the rebuild check interval.
 *
 *  @return Rebuild check interval in seconds.
 */
unsigned int cleanup::get_interval() const throw () {
  return (_interval);
}

/**
 *  Thread entry point.
 */
void cleanup::run() {
  while (!_should_exit && _interval) {
    try {
      // Open DB.
      if (!_db.open())
        throw (exceptions::msg() << "could not connect"
               " to database to perform cleanup");

      try {
        {
          QSqlQuery q(_db);
          if (!q.exec(
                   "UPDATE index_data"
                   " INNER JOIN hosts ON index_data.host_id=hosts.host_id"
                   " INNER JOIN instances ON hosts.instance_id=instances.instance_id"
                   " SET index_data.to_delete=1"
                   " WHERE instances.deleted=1"))
            throw (exceptions::msg() << "could not flag the index_data "
                   << "table to delete outdated entries: "
                   << q.lastError().text());
        }
        {
          QSqlQuery q(_db);
          if (!q.exec(
                   "DELETE FROM hosts INNER JOIN instances"
                   " ON hosts.instance_id=instances.instance_id"
                   " WHERE instances.deleted=1"))
            throw (exceptions::msg() << "could not delete outdated "
                   << "entries from the hosts table: "
                   << q.lastError().text());
        }
        {
          QSqlQuery q(_db);
          if (!q.exec(
                   "DELETE FROM modules INNER JOIN instances"
                   " ON modules.instance_id=instances.instance_id"
                   " WHERE instances.deleted=1"))
            throw (exceptions::msg() << "could not delete outdated "
                   << "entries from the modules table: "
                   << q.lastError().text());
        }
      }
      catch (...) {
        // Close DB and rethrow.
        _db.close();
        throw ;
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high) << "SQL: " << e.what();
    }
    catch (...) {
      logging::error(logging::high)
        << "SQL: unknown error occurred during cleanup";
    }

    // Sleep a while.
    time_t target(time(NULL) + _interval);
    while (!_should_exit && (target > time(NULL)))
      sleep(1);
  }
  return ;
}

/**
 *  Set the database object.
 *
 *  @param[in] db DB object to copy.
 */
void cleanup::set_db(QSqlDatabase const& db) {
  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Remove old DB.
  QSqlDatabase::removeDatabase(id);

  // Clone database.
  _db = QSqlDatabase::cloneDatabase(db, id);

  return ;
}

/**
 *  Set the rebuild check interval.
 *
 *  @param[in] interval Rebuild check interval in seconds.
 */
void cleanup::set_interval(unsigned int interval) throw () {
  _interval = interval;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void cleanup::_internal_copy(cleanup const& right) {
  // Copy DB.
  QString id;
  id.setNum((qulonglong)this, 16);
  _db = QSqlDatabase::cloneDatabase(right._db, id);

  // Copy other data.
  _interval = right._interval;

  return ;
}
