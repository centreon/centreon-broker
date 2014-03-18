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

#include <QtSql>
#include <map>
#include "com/centreon/broker/bam/configuration/db.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"
#include "com/centreon/broker/bam/sql_mapping.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] mydb       Information for accessing database.
 */
reader::reader(configuration::db const& mydb)
  : _db(), _dbinfo(mydb) {
  QString id;
  id.setNum((qulonglong)this, 16);
  _db = QSqlDatabase::addDatabase(
                               map_2_qt(QString(mydb.get_type().c_str())
                               ), id);
  _ensure_open();
}

/**
 *  Destructor.
 */
reader::~reader() {
  _db.close();
}

/**
 *  Read configuration from database.
 *
 *  @param[out] st  All the configuration state for the BA subsystem
 *                  recuperated from the specified database.
 */
void reader::read(state& st) {
  try {
    _ensure_open();
    _db.transaction(); // A single explicit transaction is more efficient
    _load( st.get_bas());
    _load( st.get_kpis());
    _load( st.get_boolexps());
    _db.commit();
  }
  catch (std::exception const& e) {
    // Apparently, no need to rollback transaction.. achieved in the db destructor.
    st.clear();
    throw ;
  }
}

/**
 *  @brief Copy constructor
 *
 *  Hidden implementation, never called.
 */
reader::reader(reader const& other) : _dbinfo(other._dbinfo) {}

/**
 *  @brief Assignment operator.
 *
 *  Hidden implementation, never called.
 *
 *  @param[in] other Unused.
 *
 *  @return This object.
 */
reader& reader::operator=(reader const& other) {
  (void)other;
  return (*this);
}

/**
 *  @brief Ensure that internal database object is open.
 *
 *  Enforce that the database be open as a postcondition.
 */
void reader::_ensure_open() {
  if (!_db.isOpen()){
    _db.setHostName(_dbinfo.get_host().c_str());
    _db.setDatabaseName(_dbinfo.get_name().c_str());
    _db.setUserName(_dbinfo.get_user().c_str());
    _db.setPassword(_dbinfo.get_password().c_str());
    // We must have a valid database connexion at this point.
    if( !_db.open()) {
      throw (reader_exception()
             << "BAM: database access failure ("
             << "reason: " <<  _db.lastError().text()
             << ", type: "  <<  _dbinfo.get_type()
             << ", host: "  <<  _dbinfo.get_host()
             << ", DB name: "  <<  _dbinfo.get_name()
             << ")");
    }
  }
}

/**
 *  Load KPIs from the DB.
 *
 *  @param[out] kpis The list of kpis in database.
 */
void reader::_load(state::kpis& kpis) {
  kpis.clear();
  QSqlQuery query =
    _db.exec("SELECT    k.kpi_id,                                              "
	     "          k.state_type,                                          "
	     "          k.host_id,                                             "
	     "          k.service_id,                                          "
	     "          k.id_ba,                                               "
	     "          k.current_status,                                      "
	     "          k.last_level,                                          "
	     "          k.downtime,                                            "
	     "          k.acknowledged,                                        "
	     "          k.ignore_downtime,                                     "
	     "          k.ignore_acknowledged,                                 "
	     "          coalesce(k.drop_warning,ww.impact),                    "
	     "          coalesce(k.drop_critical,cc.impact),                   "
	     "          coalesce(k.drop_unknown,uu.impact)                     "
	     "FROM      mod_bam_kpi AS k                                       "
	     "LEFT JOIN mod_bam_impacts AS ww ON k.drop_warning_impact_id =  ww.id_impact "
	     "LEFT JOIN mod_bam_impacts AS cc ON k.drop_critical_impact_id = cc.id_impact "
	     "LEFT JOIN mod_bam_impacts AS uu ON k.drop_unknown_impact_id =  uu.id_impact");
  _assert_query(query);

  while (query.next()) {
    kpis.push_back(
      kpi(
        query.value(0).toInt(), // ID.
        query.value(1).toInt(), // State type.
        query.value(2).toInt(), // Host ID.
        query.value(3).toInt(),// Service ID.
        query.value(4).toInt(),// BA ID.
        query.value(5).toInt(),// Status.
        query.value(6).toInt(),// Hard state.
        query.value(7).toFloat(),// Downtimed.
        query.value(8).toFloat(),// Acknowledged.
        query.value(9).toBool(),// Ignore downtime.
        query.value(10).toBool(),// Ignore acknowledgement.
        query.value(11).toDouble(),// Warning.
        query.value(12).toDouble(),// Critical.
        query.value(13).toDouble()));// Unknown.
  }
}

/**
 *  Load BAs from the DB.
 *
 *  @param[out] bas The list of bas in database.
 */
void reader::_load(state::bas& bas) {
  QSqlQuery query = _db.exec("SELECT ba_id,          "
			     "       name,           "
			     "       current_level,  "
			     "       level_w,        "
			     "       level_c,        "
			     "FROM   mod_bam");
  _assert_query(query);

  while (query.next()) {
    bas.push_back(
      ba(
        query.value(0).toInt(), // ID.
        query.value(1).toString().toStdString(), // Name.
        query.value(2).toFloat(), // Level.
        query.value(3).toFloat(), // Warning level.
        query.value(4).toFloat())); // Critical level.
  }
}

/**
 *  Load boolean expressions from the DB.
 *
 *  @param[out] bool_exps The list of bool expression in database.
 */
void reader::_load(state::bool_exps& bool_exps) {
  QSqlQuery query = _db.exec("SELECT     be.boolean_id,      "
			     "           COALESCE(be.impact,imp.impact)"
			     "           be.expression,"
			     "           be.bool_state,"
			     "           be.current_state"
			     "FROM       mod_bam_boolean as be"
			     "LEFT JOIN  mod_bam_impacts as imp ON be.impact_id =  imp.id_impact ");
  _assert_query(query);

  while (query.next()) {
    bool_exps.push_back(
      bool_expression(
        query.value(0).toInt(), // ID.
        query.value(1).toFloat(),// Impact.
        query.value(2).toString().toStdString(), // Expression.
        query.value(3).toBool(), // Impact if.
        query.value(4).toBool())); // State.
  }
}

/**
 *  @brief Assert query.
 *
 *  Ensure that the query is legitimate.
 *
 *  @param[in] query Query to assert.
 */
void reader::_assert_query(QSqlQuery& query) {
  if (!query.isActive()) {
    throw (reader_exception()
           << "BAM: could not get extract configuration from DB: "
           << query.lastError().text()
           << " (type: " <<  _dbinfo.get_type()
           << ", host: "  <<  _db.hostName()
           << ", name: "  <<  _db.databaseName()
           << ")");
  }
}
