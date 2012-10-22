/*
** Copyright 2012 Merethis
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

#include <fstream>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <sys/stat.h>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

/**
 *  Remove DB created previously.
 *
 *  @param[in] db_name DB name.
 */
void config_db_close(char const* db_name) {
  QString connection_name;
  {
    QSqlDatabase db(QSqlDatabase::database());
    connection_name = db.connectionName();
    if (db.open()) {
      std::ostringstream query;
      query << "DROP DATABASE " << db_name;
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        std::cerr << q.lastError().text().toStdString() << std::endl;
      db.close();
    }
    else
      std::cerr << db.lastError().text().toStdString() << std::endl;
  }
  QSqlDatabase::removeDatabase(connection_name);
  return ;
}

/**
 *  Connect and install a new database to the DB server.
 *
 *  @param[in] db_name DB name.
 *
 *  @return Initialized and open DB connection.
 */
QSqlDatabase config_db_open(char const* db_name) {
  // Find DB type.
  std::string db_type;
  if (!strcmp(DB_TYPE, "mysql"))
    db_type = "QMYSQL";

  // Connect to the DB.
  QSqlDatabase db(QSqlDatabase::addDatabase(db_type.c_str()));
  db.setHostName(DB_HOST);
  db.setPassword(DB_PASSWORD);
  db.setPort(strtoul(DB_PORT, NULL, 0));
  db.setUserName(DB_USER);
  if (!db.open())
    throw (exceptions::msg() << "cannot initiate DB connection to '"
           << db_name << "': "
           << db.lastError().text().toStdString().c_str());

  // Drop DB.
  {
    std::ostringstream query;
    query << "DROP DATABASE " << db_name;
    QSqlQuery q(db);
    if (!q.exec(query.str().c_str()))
      std::cerr << q.lastError().text().toStdString() << std::endl;
  }

  // Create DB.
  {
    std::ostringstream query;
    query << "CREATE DATABASE " << db_name;
    QSqlQuery q(db);
    if (!q.exec(query.str().c_str()))
      std::cerr << q.lastError().text().toStdString() << std::endl;
  }

  // Close and reopen.
  db.close();
  db.setDatabaseName(db_name);
  if (!db.open())
    throw (exceptions::msg() << "cannot reopen connection to DB '"
           << db_name << "': "
           << db.lastError().text().toStdString().c_str());

  // Read table creation script.
  QByteArray table_creation_script;
  {
    std::ifstream ifs;
    ifs.open(PROJECT_SOURCE_DIR "/sql/mysql_schema.sql");
    if (ifs.fail())
      throw (exceptions::msg()
             << "cannot open SQL table creation script");
    char buffer[1024];
    std::streamsize rb;
    ifs.read(buffer, sizeof(buffer));
    rb = ifs.gcount();
    while (rb > 0) {
      table_creation_script.append(buffer, rb);
      ifs.read(buffer, sizeof(buffer));
      rb = ifs.gcount();
    }
    ifs.close();
  }

  // Execute table creation script.
  QSqlQuery query(db);
  if (!query.exec(QString(table_creation_script)))
    throw (exceptions::msg()
           << query.lastError().text().toStdString().c_str());

  return (db);
}

/**
 *  Remove configuration files.
 *
 *  @param[in] Configuration directory.
 */
void config_remove(char const* path) {
  // Main configuration file.
  {
    std::ostringstream oss;
    oss << path << "/nagios.cfg";
    ::remove(oss.str().c_str());
  }

  // Hosts configuration file.
  {
    std::ostringstream oss;
    oss << path << "/hosts.cfg";
    ::remove(oss.str().c_str());
  }

  // Services configuration file.
  {
    std::ostringstream oss;
    oss << path << "/services.cfg";
    ::remove(oss.str().c_str());
  }

  // Misc file.
  {
    std::ostringstream oss;
    oss << path << "/misc.cfg";
    ::remove(oss.str().c_str());
  }

  // Remove directory.
  rmdir(path);

  return ;
}

/**
 *  Write configuration file from structures.
 *
 *  @param[in] path            Destination directory.
 *  @param[in] more_directives Additionnal configuration directives.
 *  @param[in] hosts           Host list.
 *  @param[in] services        Service list.
 */
void config_write(
       char const* path,
       char const* more_directives,
       std::list<host>* hosts,
       std::list<service>* services) {
  // Create base directory.
  mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);

  // Open base file.
  std::ofstream ofs;
  {
    std::ostringstream oss;
    oss << path << "/nagios.cfg";
    ofs.open(
          oss.str().c_str(),
          std::ios_base::out | std::ios_base::trunc);
    if (ofs.fail())
      throw (exceptions::msg() << "cannot open configuration file '"
             << path << "'");
  }

  // Base configuration.
  ofs << "check_result_reaper_frequency=1\n"
      << "interval_length=1\n"
      << "log_file=monitoring_engine.log\n"
      << "max_service_check_spread=1\n"
      << "max_concurrent_checks=200\n"
      << "service_inter_check_delay_method=s\n"
      << "sleep_time=0.01\n";

  // Subconfiguration files.
  std::string hosts_file;
  {
    std::ostringstream oss;
    oss << path << "/hosts.cfg";
    hosts_file = oss.str();
  }
  std::string services_file;
  {
    std::ostringstream oss;
    oss << path << "/services.cfg";
    services_file = oss.str();
  }
  std::string misc_file;
  {
    std::ostringstream oss;
    oss << path << "/misc.cfg";
    misc_file = oss.str();
  }
  ofs << "cfg_file=" << hosts_file << "\n"
      << "cfg_file=" << services_file << "\n"
      << "cfg_file=" << misc_file << "\n";

  // Additional configuration.
  ofs << MONITORING_ENGINE_ADDITIONAL;

  // Additional directives.
  if (more_directives)
    ofs << more_directives << "\n";

  // Close base file.
  ofs.close();

  // Hosts.
  ofs.open(
        hosts_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open hosts configuration file in '"
           << path << "'");
  ofs << "define host{\n"
      << "  host_name default_host\n"
      << "  alias default_host\n"
      << "  address localhost\n"
      << "  check_command default_command\n"
      << "  max_check_attempts 5\n"
      << "  check_period default_timeperiod\n"
      << "  contacts default_contact\n"
      << "}\n\n";
  if (hosts)
    for (std::list<host>::iterator
           it(hosts->begin()),
           end(hosts->end());
         it != end;
         ++it) {
      ofs << "define host{\n"
          << "  host_name " << it->name << "\n"
          << "  _HOST_ID " << it->name << "\n"
          << "  alias " << (it->alias ? it->alias : it->name) << "\n"
          << "  address " << (it->address ? it->address : "localhost")
          << "\n"
          << "  check_command " << (it->host_check_command
                                    ? it->host_check_command
                                    : "default_command") << "\n"
          << "  max_check_attempts " << ((it->max_attempts > 0)
                                         ? it->max_attempts
                                         : 5) << "\n"
          << "  check_period " << (it->check_period
                                   ? it->check_period
                                   : "default_timeperiod") << "\n"
          << "  notification_interval "
          << ((it->notification_interval > 0)
              ? it->notification_interval
              : 10) << "\n"
          << "  notification_period " << (it->notification_period
                                          ? it->notification_period
                                          : "default_timeperiod")
          << "\n"
          << "  contacts ";
      if (it->contacts) {
        ofs << it->contacts->contact_name;
        for (contactsmember* mbr(it->contacts->next);
             mbr;
             mbr = mbr->next)
          ofs << "," << mbr->contact_name;
      }
      else
        ofs << "default_contact";
      ofs << "\n";
      ofs << "}\n\n";
    }
  ofs.close();

  // Services.
  ofs.open(
        services_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open services configuration file in '"
           << path << "'");
  ofs << "define service{\n"
      << "  service_description default_service\n"
      << "  host_name default_host\n"
      << "  check_command default_command\n"
      << "  max_check_attempts 5\n"
      << "  check_interval 5\n"
      << "  retry_interval 3\n"
      << "  check_period default_timeperiod\n"
      << "  notification_interval 10\n"
      << "  notification_period default_timeperiod\n"
      << "  contacts default_contact\n"
      << "}\n\n";
  if (services)
    for (std::list<service>::iterator
           it(services->begin()),
           end(services->end());
         it != end;
         ++it) {
      ofs << "define service{\n"
          << "  service_description " << it->description << "\n"
          << "  _SERVICE_ID " << it->description << "\n"
          << "  host_name " << it->host_name << "\n"
          << "  check_command "
          << (it->service_check_command
              ? it->service_check_command
              : "default_command") << "\n"
          << "  max_check_attempts "
          << ((it->max_attempts > 0) ? it->max_attempts : 5) << "\n"
          << "  check_interval "
          << ((it->check_interval > 0) ? it->check_interval : 5) << "\n"
          << "  retry_interval "
          << ((it->retry_interval > 0) ? it->retry_interval : 5) << "\n"
          << "  check_period " << (it->check_period
                                   ? it->check_period
                                   : "default_timeperiod") << "\n"
          << "  notification_interval "
          << ((it->notification_interval > 0)
              ? it->notification_interval
              : 10) << "\n"
          << "  notification_period "
          << (it->notification_period
              ? it->notification_period
              : "default_timeperiod") << "\n"
          << "  contacts ";
      if (it->contacts) {
        ofs << it->contacts->contact_name;
        for (contactsmember* mbr(it->contacts->next);
             mbr;
             mbr = mbr->next)
          ofs << "," << mbr->contact_name;
      }
      else
        ofs << "default_contact";
      ofs << "\n";
      ofs << "}\n\n";
    }
  ofs.close();

  // Misc.
  ofs.open(
        misc_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open misc configuration file in '"
           << path << "'");
  ofs << "define command{\n"
      << "  command_name default_command\n"
      << "  command_line /bin/true\n"
      << "}\n"
      << "\n"
      << "define timeperiod{\n"
      << "  timeperiod_name default_timeperiod\n"
      << "  alias MyTimePeriod\n"
      << "  monday 00:00-24:00\n"
      << "  tuesday 00:00-24:00\n"
      << "  wednesday 00:00-24:00\n"
      << "  thursday 00:00-24:00\n"
      << "  friday 00:00-24:00\n"
      << "  saturday 00:00-24:00\n"
      << "  sunday 00:00-24:00\n"
      << "}\n"
      << "\n"
      << "define contact{\n"
      << "  contact_name default_contact\n"
      << "  host_notification_period default_timeperiod\n"
      << "  host_notification_commands default_command\n"
      << "  service_notification_period default_timeperiod\n"
      << "  service_notification_commands default_command\n"
      << "}\n"
      << "\n";

  return ;
}
