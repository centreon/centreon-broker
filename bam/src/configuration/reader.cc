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

#include "com/centreon/broker/bam/configuration/db.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"


#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"




using namespace com::centreon::broker::bam::configuration;



/**
 * Constructor
 *
 *@param[in] mydb       Information for accessing database
 *@param[out] outState  All the configuration state for the BA subsystem
 */
void reader::read(db const& mydb, state& st ){
 

  try{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(mydb.get_host().c_str()  );
    db.setDatabaseName(mydb.get_name().c_str()  );
    db.setUserName(mydb.get_user().c_str()  );
    db.setPassword(mydb.get_password().c_str()  );

    if (!db.open()) {
      throw reader_exception() << "\nDatabase Error: Access failure" 
			       << "\nReason:"<<  db.lastError().text()
			       << "\nHost:"  <<  mydb.get_host() 
			       << "\nName:"  <<  mydb.get_name() ;
    }

    db.transaction(); // c'est plus optimal d'avoir UNE transaction explicit au lieu de trois implicits
    load(db, st.get_bas() );    
    load(db, st.get_kpis() );
    load(db, st.get_boolexps());
    db.commit();
  }
  catch( std::exception& e){
    //apparently, no need to rollback transaction.. achieved in the db destructor
    st.clear(); 
    throw;
  }    

}


/**
 * constructor
 *
 */
reader::reader(){
}


/**
 * copy constructor
 *
 */
reader::reader(const reader& other){
}


/**
 * assignment operator
 *
 */
reader& reader::operator=(const reader& other){
  return *this;
}


/**
 * destructor
 *
 */
reader::~reader(){
}


void reader::load(QSqlDatabase& mydb, state::kpis& kpis){
  kpis.clear();


  QSqlQuery query = mydb.exec("SELECT    k.kpi_id,                                               "
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

  assert_query(mydb, query);
 
  while (query.next()) {      
    kpis.push_back(configuration::kpi(
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

void reader::load(QSqlDatabase& mydb, state::bas& bas){

  QSqlQuery query = mydb.exec("SELECT ba_id,          "
			      "       name,           "
			      "       current_level,  "
			      "       level_w,        "
			      "       level_c,        "
			      "FROM   mod_bam"); 

  assert_query(mydb, query);

  while (query.next()) {      
    bas.push_back(configuration::ba(
				    query.value(0).toInt(),    //unsigned int id = 0,
				    query.value(1).toString().toStdString() , //std::string const& name = "",
				    query.value(2).toFloat(),    //double level = 0.0,
				    query.value(3).toFloat(),    //double warning_level = 0.0
				    query.value(4).toFloat()    //double critical_level = 0.0);
				    ));
  }
}

void reader::load(QSqlDatabase& mydb, state::bool_exps& bool_exps){
  /*
    CREATE TABLE  `mod_bam_boolean` (
    `boolean_id` INT( 11 ) NOT NULL AUTO_INCREMENT ,
    `name` VARCHAR( 255 ) NOT NULL ,
    `config_type` TINYINT NOT NULL ,
    `impact` FLOAT NULL ,
    `impact_id` INT( 11 ) NULL ,
    `expression` TEXT NOT NULL ,
    `bool_state` BOOL NOT NULL DEFAULT '1',
    `current_state` BOOL NOT NULL DEFAULT '0',
    `comments` TEXT NULL ,
    `activate` TINYINT NOT NULL ,
    PRIMARY KEY (  `boolean_id` )
    ) ENGINE = INNODB CHARACTER SET utf8 COLLATE utf8_general_ci;
  */
  QSqlQuery query = mydb.exec("SELECT     be.boolean_id,      "
			      "           COALESCE(be.impact,imp.impact)"
			      "           be.expression,"
			      "           be.bool_state,"
			      "           be.current_state"
			      "FROM       mod_bam_boolean as be"
                              "LEFT JOIN  mod_bam_impacts as imp ON be.impact_id =  imp.id_impact "
			      ); 

  assert_query(mydb, query);

  while (query.next()) {      
    bool_exps.push_back(configuration::bool_expression(
						       query.value(0).toInt(), //unsigned int id = 0,
						       query.value(1).toFloat(),//double impact = 0.0,
						       query.value(2).toString().toStdString(),//std::string const& expression = "",
						       query.value(3).toBool(),//bool impact_if = false,
						       query.value(4).toBool()//bool state = false
						       ));
  } 

}

void reader::assert_query( QSqlDatabase& mydb,  QSqlQuery& query){
  if(!query.isActive()){
    throw reader_exception() << "\nDatabase Select Error: " 
			     << "\nReason:"<<  query.lastError().text()
			     << "\nHost:"  <<  mydb.hostName() 
			     << "\nName:"  <<  mydb.databaseName() ;
  }

}
