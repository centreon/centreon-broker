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

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include <sys/stat.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/rrd_file.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_monitoring_to_rrd"
#define HOST_COUNT 10
#define SERVICES_BY_HOST 1

/**
 *  Check that monitoring is properly inserted in RRD graphs.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<command> commands;
  std::list<host> hosts;
  std::list<service> services;
  std::string cbmod_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string metrics_path(tmpnam(NULL));
  std::string status_path(tmpnam(NULL));
  engine daemon;

  // Log.
  std::clog << "status directory: " << status_path << "\n"
            << "metrics directory: " << metrics_path << std::endl;

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Create RRD paths.
    mkdir(metrics_path.c_str(), S_IRWXU);
    mkdir(status_path.c_str(), S_IRWXU);

    // Write cbmod configuration file.
    {
      std::ofstream ofs;
      ofs.open(
            cbmod_config_path.c_str(),
            std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw (exceptions::msg()
               << "cannot open cbmod configuration file '"
               << cbmod_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR "/test/cfg/broker_modules.xml</include>\n"
          << "  <instance>42</instance>\n"
          << "  <instance_name>MyBroker</instance_name>\n"
          << "  <!--\n"
          << "  <logger>\n"
          << "    <type>file</type>\n"
          << "    <name>cbmod.log</name>\n"
          << "    <config>1</config>\n"
          << "    <debug>1</debug>\n"
          << "    <error>1</error>\n"
          << "    <info>1</info>\n"
          << "    <level>3</level>\n"
          << "  </logger>\n"
          << "  -->\n"
          << "  <output>\n"
          << "    <name>EngineToStorageUnitTest</name>\n"
          << "    <type>storage</type>\n"
          << "    <db_type>" DB_TYPE "</db_type>\n"
          << "    <db_host>" DB_HOST "</db_host>\n"
          << "    <db_port>" DB_PORT "</db_port>\n"
          << "    <db_user>" DB_USER "</db_user>\n"
          << "    <db_password>" DB_PASSWORD "</db_password>\n"
          << "    <db_name>" DB_NAME "</db_name>\n"
          << "    <queries_per_transaction>0</queries_per_transaction>\n"
          << "    <interval>" MONITORING_ENGINE_INTERVAL_LENGTH_STR "</interval>\n"
          << "    <length>2592000</length>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>StorageToRRDUnitTest</name>\n"
          << "    <type>rrd</type>\n"
          << "    <metrics_path>" << metrics_path << "</metrics_path>\n"
          << "    <status_path>" << status_path << "</status_path>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, 5);
    {
      std::list<command>::iterator it(commands.begin());
      char const* cmd;
      cmd = "echo \"ABSOLUTE|a[absolute]=22374B\\;1000\\;2000\\;0\\;3000\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = "echo \"COUNTER|c[counter]=147852369\\;123\\;745698877\\;3\\;nan\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = "echo \"DERIVE|d[derive]=89588s\\;100000\\;1000\\;\\;inf\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = "echo \"GAUGE|g[gauge]=135.25kB/s\\;\\;1100\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = "echo \"DEFAULT|default=9queries_per_second\\;9\\;10\\;0\\;100\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
    }
    generate_hosts(hosts, HOST_COUNT);
    {
      unsigned int i(0);
      for (std::list<host>::iterator
             it(hosts.begin()),
             end(hosts.end());
           it != end;
           ++it) {
        char str[2];
        str[0] = '1' + i;
        str[1] = '\0';
        it->host_check_command = new char[sizeof(str)];
        strcpy(it->host_check_command, str);
        i = (i + 1) % 5;
      }
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      unsigned int i(0);
      for (std::list<service>::iterator
             it(services.begin()),
             end(services.end());
           it != end;
           ++it) {
        char str[2];
        str[0] = '1' + i;
        str[1] = '\0';
        it->service_check_command = new char[sizeof(str)];
        strcpy(it->service_check_command, str);
        i = (i + 1) % 5;
      }
    }
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_config_path;
      cbmod_loading = oss.str();
    }

    // Insert entries in index_data.
    {
      QSqlQuery q(db);
      // Host does not have status graph (yet).
      // for (unsigned int i(0); i < HOST_COUNT; ++i) {
      //   std::ostringstream query;
      //   query << "INSERT INTO index_data (host_id, service_id)"
      //         << "  VALUES (" << i + 1 << ", NULL)";
      //   if (!q.exec(query.str().c_str()))
      //     throw (exceptions::msg() << "cannot create index of host "
      //            << i + 1 << ": " << qPrintable(q.lastError().text()));
      // }
      for (unsigned int i(1); i <= HOST_COUNT * SERVICES_BY_HOST; ++i) {
        std::ostringstream query;
        query << "INSERT INTO index_data (host_id, service_id)"
              << "  VALUES (" << i << ", " << i << ")";
        if (!q.exec(query.str().c_str()))
          throw (exceptions::msg() << "cannot create index of service ("
                 << i << ", " << i << ")");
      }
    }

    // Get index list.
    std::list<unsigned int> indexes;
    {
      QSqlQuery q(db);
      if (!q.exec("SELECT id FROM index_data"))
        throw (exceptions::msg() << "cannot get index list: "
               << qPrintable(q.lastError().text()));
      while (q.next())
        indexes.push_back(q.value(0).toUInt());
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services,
      &commands);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(60 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check metrics table.
    std::list<unsigned int> metrics;
    {
      std::ostringstream query;
      query << "SELECT m.metric_id, m.metric_name, m.data_source_type,"
            << "       m.unit_name, m.warn, m.crit, m.min, m.max"
            << "  FROM metrics AS m JOIN index_data AS i"
            << "  ON m.index_id=i.id"
            << "  ORDER BY i.host_id ASC, i.service_id ASC";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get metric list: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0); i < HOST_COUNT * SERVICES_BY_HOST; ++i) {
        if (!q.next())
          throw (exceptions::msg() << "not enough entry in metrics ("
                 << i << " expected 10)");
        metrics.push_back(q.value(0).toUInt());
        std::string metric_name(q.value(1).toString().toStdString());
        unsigned int data_source_type(q.value(2).toUInt());
        std::string unit_name(q.value(3).toString().toStdString());
        double warning(q.value(4).toDouble());
        double critical(q.value(5).toDouble());
        double min_val(q.value(6).toDouble());
        double max_val(q.value(7).toDouble());
        bool error;
        switch (i % 5) {
        case 0:
          error = ((metric_name != "absolute")
                   || (data_source_type != 3)
                   || (unit_name != "B")
                   || (fabs(warning - 1000.0) > 0.1)
                   || (fabs(critical - 2000.0) > 0.1)
                   || (fabs(min_val - 0.0) > 0.001)
                   || (fabs(max_val - 3000.0) > 0.1));
          break ;
        case 1:
          error = ((metric_name != "counter")
                   || (data_source_type != 1)
                   || (unit_name != "")
                   || (fabs(warning - 123.0) > 0.01)
                   || (fabs(critical - 745698877.0) > 100000.0)
                   || (fabs(min_val - 3.0) > 0.001)
                   || !q.value(7).isNull());
          break ;
        case 2:
          error = ((metric_name != "derive")
                   || (data_source_type != 2)
                   || (unit_name != "s")
                   || (fabs(warning - 100000.0) > 100.0)
                   || (fabs(critical - 1000.0) > 10.0)
                   || !q.value(6).isNull()
                   || (fabs(max_val - DBL_MAX - 1.0) < 0.1));
          break ;
        case 3:
          error = ((metric_name != "gauge")
                   || (data_source_type != 0)
                   || (unit_name != "kB/s")
                   || !q.value(4).isNull()
                   || (fabs(critical - 1100.0) > 1.0)
                   || !q.value(6).isNull()
                   || !q.value(7).isNull());
          break ;
        case 4:
          error = ((metric_name != "default")
                   || (data_source_type != 0)
                   || (unit_name != "queries_per_second")
                   || (fabs(warning - 9.0) > 0.0001)
                   || (fabs(critical - 10.0) > 0.0001)
                   || (fabs(min_val) > 0.0001)
                   || (fabs(max_val - 100.0) > 0.0001));
          break ;
        }
        if (error)
          throw (exceptions::msg() << "invalid metric #" << i
                 << " (type " << i % 5 << ", metric "
                 << metric_name.c_str() << ")");
      }
    }

    // Check data_bin table.
    {
      for (std::list<unsigned int>::const_iterator
             it(metrics.begin()),
             end(metrics.end());
           it != end;
           ++it) {
        std::ostringstream query;
        query << "SELECT COUNT(*)"
              << "  FROM data_bin"
              << "  WHERE id_metric=" << *it;
        QSqlQuery q(db);
        if (!q.exec(query.str().c_str())
            || !q.next()
            || !q.value(0).toUInt())
          throw (exceptions::msg() << "data_bin is invalid for metric "
                 << *it << ": " << qPrintable(q.lastError().text()));
      }
    }

    // Check that status RRD files exist.
    unsigned int i(0);
    for (std::list<unsigned int>::const_iterator
           it(indexes.begin()),
           end(indexes.end());
         it != end;
         ++it) {
      std::ostringstream path;
      path << status_path << "/" << *it << ".rrd";
      if (!QFile::exists(path.str().c_str()))
        throw (exceptions::msg() << "status RRD file '"
               << path.str().c_str() << "' does not exist");
    }

    // Check that metrics RRD files exist.
    i = 0;
    for (std::list<unsigned int>::const_iterator
           it(metrics.begin()),
           end(metrics.end());
         it != end;
         ++it, ++i) {
      std::ostringstream path;
      path << metrics_path << "/" << *it << ".rrd";
      if (!QFile::exists(path.str().c_str()))
        throw (exceptions::msg() << "metrics RRD file '"
               << path.str().c_str() << "' does not exist");
      rrd_file f;
      f.load(path.str().c_str());

      // Check content.
      if (f.get_rras().size() != 2)
        throw (exceptions::msg() << "metrics RRD file '"
               << path.str().c_str() << "' does not have two RRAs");
      if (f.get_rras().front().size() < 2)
        throw (exceptions::msg() << "metrics RRD file '"
               << path.str().c_str()
               << "' does not have enough entries in its first RRA");
      switch (i % 5) {
      case 0:
        for (std::map<time_t, double>::const_iterator
               it(f.get_rras().front().begin()),
               end(f.get_rras().front().end());
             it != end;
             ++it)
          if (fabs(it->second - 1491.6) > 0.01)
            throw (exceptions::msg()
                   << "invalid absolute value in RRD file '"
                   << path.str().c_str() << "' (entry " << i
                   << "): got " << it->second << " expected 1491.6");
        break ;
      case 1:
        for (std::map<time_t, double>::const_iterator
               it(f.get_rras().front().begin()),
               end(f.get_rras().front().end());
             it != end;
             ++it)
          if (fabs(it->second) > 0.01)
            throw (exceptions::msg()
                   << "invalid counter value in RRD file '"
                   << path.str().c_str() << "' (entry " << i
                   << "): got " << it->second << " expected 0.0");
        break ;
      case 2:
        for (std::map<time_t, double>::const_iterator
               it(f.get_rras().front().begin()),
               end(f.get_rras().front().end());
             it != end;
             ++it)
          if (fabs(it->second) > 0.01)
            throw (exceptions::msg()
                   << "invalid derive value in RRD file '"
                   << path.str().c_str() << "' (entry " << i
                   << "): got " << it->second << " expected 0.0");
        break ;
      case 3:
        for (std::map<time_t, double>::const_iterator
               it(f.get_rras().front().begin()),
               end(f.get_rras().front().end());
             it != end;
             ++it)
          if (fabs(it->second - 135.25) > 0.01)
            throw (exceptions::msg()
                   << "invalid gauge value in RRD file '"
                   << path.str().c_str() << "' (entry " << i
                   << "): got " << it->second << " expected 135.25");
        break ;
      case 4:
        for (std::map<time_t, double>::const_iterator
               it(f.get_rras().front().begin()),
               end(f.get_rras().front().end());
             it != end;
             ++it)
          if (fabs(it->second - 9.0) > 0.01)
            throw (exceptions::msg()
                   << "invalid default value in RRD file '"
                   << path.str().c_str() << "' (entry " << i
                   << "): got " << it->second << " expected 9.0");
        break ;
      }
    }

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  config_remove(engine_config_path.c_str());
  ::remove(cbmod_config_path.c_str());
  config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);
  QStringList metrics_entries(QDir(metrics_path.c_str()).entryList());
  for (QStringList::const_iterator
         it(metrics_entries.begin()),
         end(metrics_entries.end());
       it != end;
       ++it) {
    std::ostringstream file_path;
    file_path << metrics_path << "/" << it->toStdString();
    ::remove(file_path.str().c_str());
  }
  QDir().rmdir(metrics_path.c_str());
  QStringList status_entries(QDir(status_path.c_str()).entryList());
  for (QStringList::const_iterator
         it(status_entries.begin()),
         end(status_entries.end());
       it != end;
       ++it) {
    std::ostringstream file_path;
    file_path << status_path << "/" << it->toStdString();
    ::remove(file_path.str().c_str());
  }
  QDir().rmdir(status_path.c_str());

  return (retval);
}
