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

using namespace com::centreon::broker::bam::configuration;



/**
 * Constructor
 *
 */
void reader::read(db const& mydb, state& out ){
 
  /*    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.11.3");
    db.setDatabaseName("menudb");
    db.setUserName("root");
    db.setPassword("test");
    if (!db.open()) 
    {
      QMessageBox::critical(0, QObject::tr("Database Error"),
			    db.lastError().text());    
    }
    
    QSqlQuery query("SELECT * FROM test");   
    
    table->setColumnCount(query.record().count());
    table->setRowCount(query.size());
    
    int index=0;
    while (query.next()) 
    {      
	table->setItem(index,0,new QTableWidgetItem(query.value(0).toString()));
	table->setItem(index,1,new QTableWidgetItem(query.value(1).toString()));	
	index++;
    }
    
    table->show();
    return app.exec();
  */
}




