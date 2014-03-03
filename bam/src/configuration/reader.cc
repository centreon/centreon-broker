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

#include "com/centreon/broker/bam/configuration/db.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include <QtSql>

#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"

using namespace com::centreon::broker::bam::configuration;



/**
 * Constructor
 *
 *@param[in] mydb       Information for accessing database
 *@param[out] outState  All the configuration state for the BA subsystem
 */
void reader::read(db const& mydb, state& out ){
 
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName( mydb.get_host().c_str()  );
  db.setDatabaseName( mydb.get_name().c_str()  );
  db.setUserName( mydb.get_user().c_str()  );
  db.setPassword( mydb.get_password().c_str()  );

  if (!db.open()) {
    throw reader_exception() << "\nDatabase Error: Access failure" 
			     << "\nReason:"<<  db.lastError().text()
			     << "\nHost:"  <<  mydb.get_host() 
			     << "\nName:"  <<  mydb.get_name() ;
  }
    
  QSqlQuery query("SELECT *" 
                  " FROM test");   
    
  //int QSqlQuery::size () const

  //table->setColumnCount(query.record().count());
  //table->setRowCount(query.size());
    
  int index=0;
  while (query.next()) 
    {      
      // table->setItem(index,0,new QTableWidgetItem(query.value(0).toString()));
      //table->setItem(index,1,new QTableWidgetItem(query.value(1).toString()));	
      index++;
    }
    
  //table->show();
  // return app.exec();  
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
