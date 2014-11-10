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
#include <QMutexLocker>
#include <QSqlQuery>
#include <QSqlError>
#include "com/centreon/broker/bam/availability_thread.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] db_type      BAM DB type.
 *  @param[in] db_host      BAM DB host.
 *  @param[in] db_port      BAM DB port.
 *  @param[in] db_user      BAM DB user.
 *  @param[in] db_password  BAM DB password.
 *  @param[in] db_name      BAM DB name.
 */
availability_thread::availability_thread(
                       QString const& db_type,
                       QString const& db_host,
                       unsigned short db_port,
                       QString const& db_user,
                       QString const& db_password,
                       QString const& db_name)
  : _mutex(QMutex::Recursive),
    _should_exit(false),
    _should_rebuild_all(false) {
  // Availability thread connection ID.
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

  // Open database.
  if (!_db->open()) {
    QString error(_db->lastError().text());
      throw (broker::exceptions::msg()
        << "BAM-BI: Availibity could not connect to "
            "reporting database '"
        << db_name << "' on host '" << db_host
        << ":" << db_port << "': " << error);
  }
}

/**
 *  Destructor.
 */
availability_thread::~availability_thread() {
  QString bam_id;
  bam_id.setNum((qulonglong)this, 16);
  QSqlDatabase::removeDatabase(bam_id);
}

/**
 *  The main loop of thread.
 */
void availability_thread::run() {
  // Lock the mutex.
  _mutex.lock();

  // Check for termination asked.
  if (_should_exit) {
    _mutex.unlock();
    return ;
  }

  while (true) {
    // Calculate the duration until next midnight.
    time_t midnight;
    if (!_compute_next_midnight(midnight))
      break ;
    unsigned long wait_for = std::difftime(midnight, time(NULL));
    _wait.wait(&_mutex, wait_for * 1000);

    // Termination asked.
    if (_should_exit)
      break ;


    bool success = _build_availabilities(
                      _should_rebuild_all ? (time_t)-1
                                          : midnight - 3600 * 24);

    if (success)
      _should_rebuild_all = false;
  }

  // Unlock the mutex.
  _mutex.unlock();
}

/**
 *  Ask for the thread termination.
 */
void availability_thread::terminate() {
  QMutexLocker lock(&_mutex);
  _should_exit = true;
  _wait.wakeOne();
}

/**
 *  Ask the thread to rebuild the availabilities.
 */
void availability_thread::rebuild_availabilities() {
  QMutexLocker lock(&_mutex);
  _should_rebuild_all = true;
  _wait.wakeOne();
}

/**
 *  @brief  Build all the availabilities since a certain time.
 *
 *  This is called from the context of the availablity thread.
 *
 *  @param[in]  since The time.
 *
 *  @return     True if the build was successful.
 */
bool availability_thread::_build_availabilities(time_t since) {
  QString query = "SELECT a.ba_event_id, b.ba_id, a.start_time, a.end_time,"
                  "       a.duration, a.sla_duration, a.timeperiod_id,"
                  "       a.timeperiod_is_default, b.status, b.in_downtime"
                  "  FROM mod_bam_reporting_ba_events_durations AS a"
                  "    INNER JOIN mod_bam_reporting_ba_events AS b";
  if (since != time_t(-1))
    query.append("  WHERE a.start_time >= ").append(QString::number(since));
  QSqlQuery q(*_db);
  q.setForwardOnly(true);
  if (!q.exec(query)) {
    logging::error(logging::medium)
      << "BAM-BI: the availability thread could not build the data: "
      << q.lastError().text();
    return (false);
  }
  while (q.next()) {

  }

  return (true);
}

/**
 *  Compute the next midnight.
 *
 *  @param[out] res  The next midnight.
 *
 *  @return  True if the next midnight was found, false otherwise.
 */
bool availability_thread::_compute_next_midnight(time_t& res) {
  time_t now = std::time(NULL);
  struct tm tmv;
  if (!localtime_r(&now, &tmv))
    return (false);
  tmv.tm_sec = 1;
  tmv.tm_min = tmv.tm_hour = 0;
  time_t current_midnight = mktime(&tmv);
  if (current_midnight == (time_t)-1)
    return (false);
  res = current_midnight + 24 * 3600;
  return (true);
}
