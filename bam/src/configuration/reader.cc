/*
** Copyright 2009-2014 Merethis
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

using namespace com::centreon::broker::bam::configuration;


/**
 *  @class  create_map
 *  @brief  A clever piece of code found on the net to
 *          automate the loading of a map from a literal expression
 *          .... why write it yourself? ... This sort of code needs a seperate namespace..
 */
template <typename T, typename U>
class create_map
{
public:
    create_map(const T& key, const U& val)
    {
        m_map[key] = val;
    }

    create_map<T, U>& operator()(const T& key, const U& val)
    {
        m_map[key] = val;
        return *this;
    }

    operator std::map<T, U>()
    {
        return m_map;
    }
private:
    std::map<T, U> m_map;
};

/**
 *  @function map_2_QT
 *  @brief   maps the logical name for a RDBMS to the Qlib name
 *  @param[in]  The logical name for the database
 *  @return     The QT lib name for the database system
 */
 QString map_2_QT(const std::string& dbtype){

    using namespace std;

// lower case the string
    QString qt_db_type(dbtype.c_str());
    qt_db_type = qt_db_type.toLower();

    // load map only ONCE.. at first execution
    // avantage    :  logN, typesafe, thread-safe ( AFTER init)
    // disavantage :  race on initialisation...
    typedef map<QString, QString> string_2_string;
    static string_2_string name_2_qname=
      create_map<QString,QString>
      ("db2",        "QDB2")
      ("ibase",      "QIBASE")
      ("interbase",  "QIBASE")
      ("mysql",      "QMYSQL")
      ("oci",        "QOCI")
      ("oracle",     "QOCI")
      ("odbc",       "QODBC")
      ("psql",       "QPSQL")
      ("postgres",   "QPSQL")
      ("postgresql", "QPSQL")
      ("sqlite",     "QSQLITE")
      ("tds",        "QTDS")
      ("sybase",     "QTDS");

    // find the database in table
    string_2_string::iterator found = name_2_qname.find(qt_db_type);
    return (found != name_2_qname.end() ? found->first : qt_db_type);
}

/**
 *  Constructor.
 *
 *  @param[in] mydb       Information for accessing database.
 */
reader::reader(configuration::db const& mydb)
  : _db(),
    _dbinfo(mydb){
  QString id;
  id.setNum((qulonglong)this, 16);
  _db = QSqlDatabase::addDatabase( map_2_QT(mydb.get_type()),
                                   id);
  _ensure_open();
}

/**
 *  Destructor.
 */
reader::~reader() {
  _db.close();
}

/**
 *  Reader
 *
 *  @param[out] st  All the configuration state for the BA subsystem
 *                  recuperated from the specified database
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
  catch (std::exception& e) {
    //apparently, no need to rollback transaction.. achieved in the db destructor
    st.clear();
    throw;
  }
}

/**
 *  Copy constructor
 *
 *  @Brief   Hidden implementation
 */
reader::reader(reader const& other) :
  _dbinfo(other._dbinfo){
}

/**
 *  assignment operator
 *
 *  @Brief   Hidden implementation
 *
 */
reader& reader::operator=(reader const& other){
  (void)other;
  return (*this);
}

/**
 *  open
 *
 *  @brief   Enforce that the database be open as a postcondition
 */
void reader::_ensure_open(){
  if(!_db.isOpen()){
    _db.setHostName(_dbinfo.get_host().c_str()  );
    _db.setDatabaseName(_dbinfo.get_name().c_str()  );
    _db.setUserName(_dbinfo.get_user().c_str()  );
    _db.setPassword(_dbinfo.get_password().c_str()  );
    // we must have a valid database connexion at this point
    if( !_db.open() ){
      throw (reader_exception()
	     << "BAM: Database access failure"
	     << ", Reason: "<<  _db.lastError().text()
             << ", Type: "  <<  _dbinfo.get_type()
	     << ", Host: "  <<  _dbinfo.get_host()
	     << ", Name: "  <<  _dbinfo.get_name());
    }
  }
}

/**
 *  Load
 *
 *  @param[out] list of kpis in database
 */
void reader::_load(state::kpis& kpis){
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
	     "LEFT JOIN mod_bam_impacts AS uu ON k.drop_unknown_impact_id =  uu.id_impact;");
  _assert_query(query);

  while (query.next()) {
    kpis.push_back(
      kpi(
	  query.value(0).toInt(), //unsigned int id = 0,
	  query.value(1).toInt(), //short state_type = 0,
	  query.value(2).toInt(), //unsigned int hostid = 0,
	  query.value(3).toInt(),//unsigned int serviceid = 0
	  query.value(4).toInt(),//unsigned int ba = 0,
	  query.value(5).toInt(),//short status = 0,
	  query.value(6).toInt(),//short lasthardstate = 0,
	  query.value(7).toFloat(),//bool downtimed = false,
	  query.value(8).toFloat(),//bool acknowledged = false,
	  query.value(9).toBool(),//bool ignoredowntime = false,
	  query.value(10).toBool(),//bool ignoreacknowledgement = false,
	  query.value(11).toDouble(),//double warning = 0,
	  query.value(12).toDouble(),//double critical = 0,
	  query.value(13).toDouble()//double unknown = 0);
	  ));
  }
}

/**
 *  Load
 *
 *  @param[out] list of bas in database
 */
void reader::_load(state::bas& bas){

  QSqlQuery query = _db.exec("SELECT ba_id,          "
			     "       name,           "
			     "       current_level,  "
			     "       level_w,        "
			     "       level_c,        "
			     "FROM   mod_bam");
  _assert_query(query);

  while (query.next()) {
    bas.push_back(
      ba(query.value(0).toInt(),                  //unsigned int id = 0,
	 query.value(1).toString().toStdString() ,//std::string const& name = "",
	 query.value(2).toFloat(),                //double level = 0.0,
	 query.value(3).toFloat(),                //double warning_level = 0.0
	 query.value(4).toFloat()                 //double critical_level = 0.0);
	 ));
  }
}

/**
 *  Load
 *
 *  @param[out] list of bool expression in database
 */
void reader::_load(state::bool_exps& bool_exps){
  QSqlQuery query = _db.exec("SELECT     be.boolean_id,      "
			     "           COALESCE(be.impact,imp.impact)"
			     "           be.expression,"
			     "           be.bool_state,"
			     "           be.current_state"
			     "FROM       mod_bam_boolean as be"
			     "LEFT JOIN  mod_bam_impacts as imp ON be.impact_id =  imp.id_impact "
			     );
  _assert_query(query);

  while (query.next()) {
    bool_exps.push_back(
			bool_expression(
					query.value(0).toInt(), //unsigned int id = 0,
					query.value(1).toFloat(),//double impact = 0.0,
					query.value(2).toString().toStdString(),//std::string const& expression = "",
					query.value(3).toBool(),//bool impact_if = false,
					query.value(4).toBool()//bool state = false
					));
  }
}

/**
 *  Assert query
 *
 *  @param[in] assert that the query succeeded
 */
void reader::_assert_query(QSqlQuery& query){
  if(!query.isActive()){
    throw reader_exception() << "Database Select Error: " << query.lastError().text()
                             << ", Type: " <<  _dbinfo.get_type()
			     << ", Host:"  <<  _db.hostName()
			     << ", Name:"  <<  _db.databaseName() ;
  }
}
