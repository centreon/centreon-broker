/*
** Copyright 2012-2014 Merethis
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

#include <cstdlib>
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
 *  Default constructor.
 */
test_db::test_db() {}

/**
 *  Destructor.
 */
test_db::~test_db() {
  close();
}

/**
 *  Get the BI database.
 *
 *  @return BI database object.
 */
QSqlDatabase* test_db::bi_db() {
  return (_bi.get());
}

/**
 *  Get the Centreon database.
 *
 *  @return Centreon database object.
 */
QSqlDatabase* test_db::centreon_db() {
  return (_centreon.get());
}

/**
 *  Close databases.
 */
void test_db::close() {
  if (_centreon.get())
    _close(_centreon);
  if (_bi.get())
    _close(_bi);
  return ;
}

/**
 *  Connect and install a new database to the DB server.
 *
 *  @param[in] centreon_db_name  Centreon DB name.
 *  @param[in] bi_db_name        Centreon BI DB name.
 */
void test_db::open(
                char const* centreon_db_name,
                char const* bi_db_name) {
  // Close previous connection.
  close();

  // Find DB type.
  QString db_type;
  if (!strcmp(DB_TYPE, "mysql"))
    db_type = "QMYSQL";

  // Set connection names.
  QString centreon_connection;
  QString bi_connection;
  {
    std::ostringstream oss;
    oss << this;
    centreon_connection = oss.str().c_str();
    bi_connection = centreon_connection;
    centreon_connection.append("_centreon");
    bi_connection.append("_bi");
  }

  // Open Centreon DB.
  if (centreon_db_name) {
    _centreon.reset(new QSqlDatabase(QSqlDatabase::addDatabase(
                                                     db_type,
                                                     centreon_connection)));
    _open(*_centreon, centreon_db_name);
    _run_script(*_centreon, PROJECT_SOURCE_DIR "/sql/mysql_schema.sql");
    _run_script(*_centreon, PROJECT_SOURCE_DIR "/bam/mysql_schema_centreon.sql");
  }
  // Open Centreon BI DB.
  if (bi_db_name) {
    _bi.reset(new QSqlDatabase(QSqlDatabase::addDatabase(
                                               db_type,
                                               bi_connection)));
    _open(*_bi, bi_db_name);
    _run_script(*_bi, PROJECT_SOURCE_DIR "/bam/mysql_schema_bi.sql");
  }

  return ;
}

/**
 *  Close a single database.
 *
 *  @param[in] db  Database.
 */
void test_db::_close(std::auto_ptr<QSqlDatabase>& db) {
  QString connection_name(db->connectionName());

  {
    std::ostringstream query;
    query << "DROP DATABASE " << db->databaseName().toStdString();
    QSqlQuery q(*db);
    if (!q.exec(query.str().c_str()))
      std::cerr << q.lastError().text().toStdString() << std::endl;
  }

  db->close();
  db.reset();
  QSqlDatabase::removeDatabase(connection_name);

  return ;
}

/**
 *  Create and open some database.
 *
 *  @param[out] db       Database object.
 *  @param[in]  db_name  Database name.
 */
void test_db::_open(
                QSqlDatabase& db,
                char const* db_name) {
  // Connect to the DB.
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
    if ("QMYSQL" == db.driverName())
      query << " DEFAULT CHARACTER SET utf8";
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

  return ;
}

/**
 *  Run a script on a database.
 *
 *  @param[in,out] db           Database object.
 *  @param[in]     script_name  Path to the script.
 */
void test_db::_run_script(QSqlDatabase& db, char const* script_name) {
  // Read table creation script.
  QByteArray table_creation_script;
  {
    std::ifstream ifs;
    ifs.open(script_name);
    if (ifs.fail())
      throw (exceptions::msg()
             << "cannot open SQL table creation script '"
             << script_name << "'");
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

  return ;
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

  // Commands file.
  {
    std::ostringstream oss;
    oss << path << "/commands.cfg";
    ::remove(oss.str().c_str());
  }

  // Host groups file.
  {
    std::ostringstream oss;
    oss << path << "/host_groups.cfg";
    ::remove(oss.str().c_str());
  }

  // Service groups files.
  {
    std::ostringstream oss;
    oss << path << "/service_groups.cfg";
    ::remove(oss.str().c_str());
  }

  // Host dependencies file.
  {
    std::ostringstream oss;
    oss << path << "/host_dependencies.cfg";
    ::remove(oss.str().c_str());
  }

  // Service dependencies file.
  {
    std::ostringstream oss;
    oss << path << "/service_dependencies.cfg";
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
 *  @param[in] commands        Command list.
 *  @param[in] host_groups     Host group list.
 *  @param[in] service_groups  Service group list.
 *  @param[in] host_deps       Host dependencies.
 *  @param[in] service_deps    Service dependencies.
 */
void config_write(
       char const* path,
       char const* more_directives,
       std::list<host>* hosts,
       std::list<service>* services,
       std::list<command>* commands,
       std::list<hostgroup>* host_groups,
       std::list<servicegroup>* service_groups,
       std::list<hostdependency>* host_deps,
       std::list<servicedependency>* service_deps) {
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
  ofs << "accept_passive_host_checks=1\n"
      << "accept_passive_service_checks=1\n"
      << "check_result_path=.\n"
      << "check_result_reaper_frequency=1\n"
      << "command_file=monitoring_engine.cmd\n"
      << "event_broker_options=-1\n"
      << "execute_host_checks=1\n"
      << "execute_service_checks=1\n"
      << "interval_length=" MONITORING_ENGINE_INTERVAL_LENGTH_STR "\n"
      << "log_file=monitoring_engine.log\n"
      << "max_service_check_spread=1\n"
      << "max_concurrent_checks=200\n"
      << "service_inter_check_delay_method=s\n"
      << "sleep_time=0.01\n"
      << "state_retention_file=monitoring_engine_retention.dat\n"
      << "status_file=monitoring_engine_status.dat\n"
      << "service_inter_check_delay_method=n\n"
      << "host_inter_check_delay_method=n\n"
      << "temp_file=monitoring_engine.tmp\n"
      << "temp_path=.\n";

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
  std::string commands_file;
  {
    std::ostringstream oss;
    oss << path << "/commands.cfg";
    commands_file = oss.str();
  }
  std::string host_groups_file;
  {
    std::ostringstream oss;
    oss << path << "/host_groups.cfg";
    host_groups_file = oss.str();
  }
  std::string service_groups_file;
  {
    std::ostringstream oss;
    oss << path << "/service_groups.cfg";
    service_groups_file = oss.str();
  }
  std::string host_dependencies_file;
  {
    std::ostringstream oss;
    oss << path << "/host_dependencies.cfg";
    host_dependencies_file = oss.str();
  }
  std::string service_dependencies_file;
  {
    std::ostringstream oss;
    oss << path << "/service_dependencies.cfg";
    service_dependencies_file = oss.str();
  }
  std::string misc_file;
  {
    std::ostringstream oss;
    oss << path << "/misc.cfg";
    misc_file = oss.str();
  }
  ofs << "cfg_file=" << hosts_file << "\n"
      << "cfg_file=" << services_file << "\n"
      << "cfg_file=" << commands_file << "\n"
      << "cfg_file=" << host_groups_file << "\n"
      << "cfg_file=" << service_groups_file << "\n"
      << "cfg_file=" << host_dependencies_file << "\n"
      << "cfg_file=" << service_dependencies_file << "\n"
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
      << "  max_check_attempts 3\n"
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
          << "  active_checks_enabled " << it->checks_enabled << "\n"
          << "  passive_checks_enabled "
          << it->accept_passive_host_checks << "\n"
          << "  check_command " << (it->host_check_command
                                    ? it->host_check_command
                                    : "default_command") << "\n"
          << "  max_check_attempts " << ((it->max_attempts > 0)
                                         ? it->max_attempts
                                         : 3) << "\n"
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
      if (it->parent_hosts) {
        ofs << "  parents " << it->parent_hosts->host_name;
        for (hostsmember* parent(it->parent_hosts->next);
             parent;
             parent = parent->next)
          ofs << "," << parent->host_name;
        ofs << "\n";
      }
      ofs << "  event_handler_enabled " << it->event_handler_enabled
          << "\n";
      if (it->event_handler)
        ofs << "  event_handler " << it->event_handler << "\n";
      for (customvariablesmember* cvar(it->custom_variables);
           cvar;
           cvar = cvar->next)
        ofs << "  _" << cvar->variable_name << " "
            << cvar->variable_value << "\n";
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
      << "  max_check_attempts 3\n"
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
          << "  active_checks_enabled " << it->checks_enabled << "\n"
          << "  passive_checks_enabled "
          << it->accept_passive_service_checks << "\n"
          << "  check_command "
          << (it->service_check_command
              ? it->service_check_command
              : "default_command") << "\n"
          << "  max_check_attempts "
          << ((it->max_attempts > 0) ? it->max_attempts : 3) << "\n"
          << "  check_interval "
          << ((it->check_interval > 0) ? it->check_interval : 5) << "\n"
          << "  retry_interval "
          << ((it->retry_interval > 0) ? it->retry_interval : 3) << "\n"
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
      ofs << "  event_handler_enabled " << it->event_handler_enabled
          << "\n";
      if (it->event_handler)
        ofs << "  event_handler " << it->event_handler << "\n";
      for (customvariablesmember* cvar(it->custom_variables);
           cvar;
           cvar = cvar->next)
        ofs << "  _" << cvar->variable_name << " "
            << cvar->variable_value << "\n";
      ofs << "}\n\n";
    }
  ofs.close();

  // Commands.
  ofs.open(
        commands_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open commands configuration file in '"
           << path << "'");
  ofs << "define command{\n"
      << "  command_name default_command\n"
      << "  command_line " MY_PLUGIN_PATH " 0\n"
      << "}\n\n";
  if (commands)
    for (std::list<command>::iterator
           it(commands->begin()),
           end(commands->end());
         it != end;
         ++it) {
      ofs << "define command{\n"
          << "  command_name " << it->name << "\n"
          << "  command_line "
          << (it->command_line ? it->command_line : MY_PLUGIN_PATH " 0") << "\n"
          << "}\n\n";
    }
  ofs.close();

  // Host groups.
  ofs.open(
        host_groups_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open host groups configuration file in '"
           << path << "'");
  if (host_groups)
    for (std::list<hostgroup>::iterator
           it(host_groups->begin()),
           end(host_groups->end());
         it != end;
         ++it) {
      ofs << "define hostgroup{\n"
          << "  hostgroup_name " << it->group_name << "\n";
      if (it->action_url)
        ofs << "  action_url " << it->action_url << "\n";
      if (it->alias)
        ofs << "  alias " << it->alias << "\n";
      if (it->notes)
        ofs << "  notes " << it->notes << "\n";
      if (it->notes_url)
        ofs << "  notes_url " << it->notes_url << "\n";
      if (it->members) {
        ofs << "  members " << it->members->host_name;
        for (hostsmember* m(it->members->next); m; m = m->next)
          ofs << "," << m->host_name;
        ofs << "\n";
      }
      ofs << "}\n\n";
    }
  ofs.close();

  // Service groups.
  ofs.open(
        service_groups_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open service groups configuration file in '"
           << path << "'");
  if (service_groups)
    for (std::list<servicegroup>::iterator
           it(service_groups->begin()),
           end(service_groups->end());
         it != end;
         ++it) {
      ofs << "define servicegroup{\n"
          << "  servicegroup_name " << it->group_name << "\n";
      if (it->action_url)
        ofs << "  action_url " << it->action_url << "\n";
      if (it->alias)
        ofs << "  alias " << it->alias << "\n";
      if (it->notes)
        ofs << "  notes " << it->notes << "\n";
      if (it->notes_url)
        ofs << "  notes_url " << it->notes_url << "\n";
      if (it->members) {
        ofs << "  members " << it->members->host_name
            << "," << it->members->service_description;
        for (servicesmember* m(it->members->next); m; m = m->next)
          ofs << "," << m->host_name << "," << m->service_description;
        ofs << "\n";
      }
      ofs << "}\n\n";
    }
  ofs.close();

  // Host dependencies.
  ofs.open(
        host_dependencies_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open host dependencies configuration file in '"
           << path << "'");
  if (host_deps)
    for (std::list<hostdependency>::iterator
           it(host_deps->begin()),
           end(host_deps->end());
         it != end;
         ++it) {
      ofs << "define hostdependency{\n"
          << "  dependent_host_name " << it->dependent_host_name << "\n"
          << "  host_name " << it->host_name << "\n"
          << "  notification_failure_criteria d,u\n"
          << "}\n\n";
    }
  ofs.close();

  // Service dependencies.
  ofs.open(
        service_dependencies_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open service dependencies configuration file in '"
           << path << "'");
  if (service_deps)
    for (std::list<servicedependency>::iterator
           it(service_deps->begin()),
           end(service_deps->end());
         it != end;
         ++it) {
      ofs << "define servicedependency{\n"
          << "  dependent_host_name " << it->dependent_host_name << "\n"
          << "  dependent_service_description "
          << it->dependent_service_description << "\n"
          << "  host_name " << it->host_name << "\n"
          << "  service_description " << it->service_description << "\n"
          << "  notification_failure_criteria w,c,u\n"
          << "}\n\n";
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
  ofs << "define timeperiod{\n"
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
