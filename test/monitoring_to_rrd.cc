/*
** Copyright 2012-2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <sys/stat.h>
#include <unistd.h>
#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextCodec>
#include <QVariant>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/rrd_file.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_monitoring_to_rrd"
#define HOST_COUNT 12
#define SERVICES_BY_HOST 1

/**
 *  Check that monitoring is properly inserted in RRD graphs.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Return value.
  int retval(EXIT_FAILURE);

  // Initialize Qt.
  QCoreApplication app(argc, argv);
  QTextCodec* utf8_codec(QTextCodec::codecForName("UTF-8"));
  if (utf8_codec)
    QTextCodec::setCodecForCStrings(utf8_codec);
  else
    std::cout << "could not set the UTF-8 codec\n";

  // Variables that need cleaning.
  std::list<command> commands;
  std::list<host> hosts;
  std::list<service> services;
  std::string cbmod_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string metrics_path(tmpnam(NULL));
  std::string status_path(tmpnam(NULL));
  engine_extcmd commander;
  engine daemon;
  test_db db;

  // Log.
  std::clog << "status directory: " << status_path << "\n"
            << "metrics directory: " << metrics_path << std::endl;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Create RRD paths.
    mkdir(metrics_path.c_str(), S_IRWXU);
    mkdir(status_path.c_str(), S_IRWXU);

    // Write cbmod configuration file.
    {
      std::ofstream ofs;
      ofs.open(cbmod_config_path.c_str(),
               std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw(exceptions::msg() << "cannot open cbmod configuration file '"
                                << cbmod_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR
             "/test/cfg/broker_modules.xml</include>\n"
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
    generate_commands(commands, 6);
    {
      std::list<command>::iterator it(commands.begin());
      char const* cmd;
      cmd = MY_PLUGIN_PATH " 0 \"NONE\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      cmd = MY_PLUGIN_PATH
          " 0 \"ABSOLUTE|a[absolute]=22374B\\;1000\\;2000\\;0\\;3000\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = MY_PLUGIN_PATH
          " 0 \"COUNTER|c[counter]=147852369\\;100:123\\;745698877\\;3\\;nan\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = MY_PLUGIN_PATH
          " 0 \"DERIVE|d[dérîve]=inf\\;100000\\;@100:1000\\;\\;-inf\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = MY_PLUGIN_PATH
          " 0 "
          "\"GAUGE|g[gauge]=135.25$_HOSTPERFDATA_UNIT$\\;$_HOSTPERFDATA_"
          "WARNING$\\;$_HOSTPERFDATA_CRITICAL$\\;$_HOSTPERFDATA_MIN$\\;$_"
          "HOSTPERFDATA_MAX$\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
      ++it;
      cmd = MY_PLUGIN_PATH
          " 0 \"DEFAULT|'my ''default'' 1337 m3tric "
          "n4m3'=9queries_per_second\\;@10:\\;@5:\\;0\\;100\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
    }
    generate_hosts(hosts, HOST_COUNT);
    {
      uint32_t i(0);
      for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
           it != end; ++it) {
        char str[2];
        str[0] = '1' + i;
        str[1] = '\0';
        it->host_check_command = new char[sizeof(str)];
        strcpy(it->host_check_command, str);
        i = (i + 1) % 6;
        set_custom_variable(*it, "PERFDATA_UNIT", "");
        set_custom_variable(*it, "PERFDATA_WARNING", "");
        set_custom_variable(*it, "PERFDATA_CRITICAL", "900");
        set_custom_variable(*it, "PERFDATA_MIN", "");
        set_custom_variable(*it, "PERFDATA_MAX", "100000");
      }
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      uint32_t i(0);
      for (std::list<service>::iterator it(services.begin()),
           end(services.end());
           it != end; ++it) {
        char str[2];
        str[0] = '1' + i;
        str[1] = '\0';
        it->service_check_command = new char[sizeof(str)];
        strcpy(it->service_check_command, str);
        i = (i + 1) % 6;
      }
    }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << cbmod_config_path << "\n";
      additional_config = oss.str();
    }

    // Insert entries in index_data.
    {
      QSqlQuery q(*db.centreon_db());
      // Host does not have status graph (yet).
      // for (uint32_t i(0); i < HOST_COUNT; ++i) {
      //   std::ostringstream query;
      //   query << "INSERT INTO rt_index_data (host_id, service_id)"
      //         << "  VALUES (" << i + 1 << ", NULL)";
      //   if (!q.exec(query.str().c_str()))
      //     throw (exceptions::msg() << "cannot create index of host "
      //            << i + 1 << ": " << qPrintable(q.lastError().text()));
      // }
      for (uint32_t i(1); i <= HOST_COUNT * SERVICES_BY_HOST; ++i) {
        std::ostringstream query;
        query << "INSERT INTO rt_index_data (host_id, service_id)"
              << "  VALUES (" << i << ", " << i << ")";
        if (!q.exec(query.str().c_str()))
          throw(exceptions::msg()
                << "cannot create index of service (" << i << ", " << i << ")");
      }
    }

    // Get index list.
    std::list<uint32_t> indexes;
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT index_id FROM rt_index_data"))
        throw(exceptions::msg()
              << "cannot get index list: " << qPrintable(q.lastError().text()));
      while (q.next())
        indexes.push_back(q.value(0).toUInt());
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services, &commands);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(15);

    // Make the metrics table update.
    for (unsigned long i(1); i <= HOST_COUNT; ++i) {
      {
        std::ostringstream oss;
        oss << "CHANGE_CUSTOM_HOST_VAR;" << i << ";PERFDATA_UNIT;kB/s";
        commander.execute(oss.str());
      }
      {
        std::ostringstream oss;
        oss << "CHANGE_CUSTOM_HOST_VAR;" << i << ";PERFDATA_WARNING;452:1099";
        commander.execute(oss.str());
      }
      {
        std::ostringstream oss;
        oss << "CHANGE_CUSTOM_HOST_VAR;" << i << ";PERFDATA_CRITICAL;1100";
        commander.execute(oss.str());
      }
      {
        std::ostringstream oss;
        oss << "CHANGE_CUSTOM_HOST_VAR;" << i << ";PERFDATA_MIN;0";
        commander.execute(oss.str());
      }
      {
        std::ostringstream oss;
        oss << "CHANGE_CUSTOM_HOST_VAR;" << i << ";PERFDATA_MAX;123456789";
        commander.execute(oss.str());
      }
    }

    // Let engine run a little more.
    sleep_for(15);

    // Check metrics table.
    std::list<uint32_t> metrics;
    {
      std::ostringstream query;
      query << "SELECT m.metric_id, m.metric_name, m.data_source_type,"
            << "       m.unit_name, m.warn, m.warn_low, "
            << "       m.warn_threshold_mode, m.crit, m.crit_low, "
            << "       m.crit_threshold_mode, m.min, m.max"
            << "  FROM rt_metrics AS m INNER JOIN rt_index_data AS i"
            << "  ON m.index_id=i.index_id"
            << "  ORDER BY i.host_id ASC, i.service_id ASC";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot get metric list: "
                                << qPrintable(q.lastError().text()));
      for (uint32_t i(0); i < HOST_COUNT * SERVICES_BY_HOST; ++i) {
        if (!(i % 6))
          continue;
        if (!q.next())
          throw(exceptions::msg()
                << "not enough entry in metrics (" << i << " expected 10)");
        metrics.push_back(q.value(0).toUInt());
        QString metric_name(q.value(1).toString());
        uint32_t data_source_type(q.value(2).toUInt());
        QString unit_name(q.value(3).toString());
        double warning(q.value(4).toDouble());
        double warning_low(q.value(5).toDouble());
        bool warning_mode(q.value(6).toUInt());
        double critical(q.value(7).toDouble());
        double critical_low(q.value(8).toDouble());
        bool critical_mode(q.value(9).toUInt());
        double min_val(q.value(10).toDouble());
        double max_val(q.value(11).toDouble());
        bool error;
        switch (i % 6) {
          case 1:
            error = ((metric_name != "absolute") || (data_source_type != 3) ||
                     (unit_name != "B") || (fabs(warning - 1000.0) > 0.1) ||
                     (fabs(warning_low) > 0.1) || warning_mode ||
                     (fabs(critical - 2000.0) > 0.1) ||
                     (fabs(critical_low) > 0.1) || critical_mode ||
                     (fabs(min_val - 0.0) > 0.001) ||
                     (fabs(max_val - 3000.0) > 0.1));
            break;
          case 2:
            error = ((metric_name != "counter") || (data_source_type != 1) ||
                     (unit_name != "") || (fabs(warning - 123.0) > 0.01) ||
                     (fabs(warning_low - 100.0) > 0.01) || warning_mode ||
                     (fabs(critical - 745698877.0) > 100000.0) ||
                     (fabs(critical_low) > 0.01) || critical_mode ||
                     (fabs(min_val - 3.0) > 0.001) || !q.value(11).isNull());
            break;
          case 3:
            error = ((metric_name != "dérîve") || (data_source_type != 2) ||
                     (unit_name != "") || (fabs(warning - 100000.0) > 100.0) ||
                     (fabs(warning_low) > 0.001) || warning_mode ||
                     (fabs(critical - 1000.0) > 10.0) ||
                     (fabs(critical_low - 100.0) > 1.0) || !critical_mode ||
                     !q.value(10).isNull() ||
                     (fabs(max_val - DBL_MAX - 1.0) < 0.1));
            break;
          case 4:
            error = ((metric_name != "gauge") || (data_source_type != 0) ||
                     (unit_name != "kB/s") || (fabs(warning - 1099.0) > 1.0) ||
                     (fabs(warning_low - 452.0) > 0.4) || warning_mode ||
                     (fabs(critical - 1100.0) > 1.0) ||
                     (fabs(critical_low) > 0.01) || critical_mode ||
                     (fabs(min_val) > 0.01) ||
                     (fabs(max_val - 123456789.0) > 100000.0));
            break;
          case 5:
            error =
                ((metric_name != "my 'default' 1337 m3tric n4m3") ||
                 (data_source_type != 0) ||
                 (unit_name != "queries_per_second") ||
                 (warning < DBL_MAX - 1.0) ||
                 (fabs(warning_low - 10.0) > 0.1) || !warning_mode ||
                 (critical < DBL_MAX - 1.0) ||
                 (fabs(critical_low - 5.0) > 0.01) || !critical_mode ||
                 (fabs(min_val) > 0.0001) || (fabs(max_val - 100.0) > 0.0001));
            break;
        }
        if (error)
          throw(exceptions::msg()
                << "invalid metric #" << i << " (type " << i % 6 << ", metric "
                << metric_name << ", data source type " << data_source_type
                << ", unit name " << unit_name << ", warning " << warning
                << ", warning low " << warning_low << ", warning mode "
                << warning_mode << ", critical " << critical
                << ", critical low " << critical_low << ", critical mode "
                << critical_mode << ", min " << min_val << ", max " << max_val
                << ")");
      }
    }

    // Check data_bin table.
    {
      for (std::list<uint32_t>::const_iterator it(metrics.begin()),
           end(metrics.end());
           it != end; ++it) {
        std::ostringstream query;
        query << "SELECT COUNT(*)"
              << "  FROM log_data_bin"
              << "  WHERE metric_id=" << *it;
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query.str().c_str()) || !q.next() || !q.value(0).toUInt())
          throw(exceptions::msg()
                << "log_data_bin is invalid for metric " << *it << ": "
                << qPrintable(q.lastError().text()));
      }
    }

    // Check that status RRD files exist.
    uint32_t i(0);
    for (std::list<uint32_t>::const_iterator it(indexes.begin()),
         end(indexes.end());
         it != end; ++it) {
      std::ostringstream path;
      path << status_path << "/" << *it << ".rrd";
      if (access(path.str().c_str(), F_OK))
        throw(exceptions::msg() << "status RRD file '" << path.str().c_str()
                                << "' does not exist");
    }

    // Check that metrics RRD files exist.
    i = 0;
    for (std::list<uint32_t>::const_iterator it(metrics.begin()),
         end(metrics.end());
         it != end; ++it, ++i) {
      std::ostringstream path;
      path << metrics_path << "/" << *it << ".rrd";
      if (access(path.str().c_str(), F_OK))
        throw(exceptions::msg() << "metrics RRD file '" << path.str().c_str()
                                << "' does not exist");
      rrd_file f;
      f.load(path.str().c_str());

      // Check content.
      if (f.get_rras().size() != 2)
        throw(exceptions::msg() << "metrics RRD file '" << path.str().c_str()
                                << "' does not have two RRAs");
      if (f.get_rras().front().size() < 2)
        throw(exceptions::msg()
              << "metrics RRD file '" << path.str().c_str()
              << "' does not have enough entries in its first RRA");
      switch (i % 5) {
        case 0:
          //   for (std::map<time_t, double>::const_iterator
          //          it(f.get_rras().front().begin()),
          //          end(f.get_rras().front().end());
          //        it != end;
          //        ++it)
          //     if (fabs(it->second - 1491.6) > 0.01)
          //       throw (exceptions::msg()
          //              << "invalid absolute value in RRD file '"
          //              << path.str().c_str() << "' (entry " << i
          //              << "): got " << it->second << " expected 1491.6");
          break;
        case 1:
          //   for (std::map<time_t, double>::const_iterator
          //          it(f.get_rras().front().begin()),
          //          end(f.get_rras().front().end());
          //        it != end;
          //        ++it)
          //     if (fabs(it->second) > 0.01)
          //       throw (exceptions::msg()
          //              << "invalid counter value in RRD file '"
          //              << path.str().c_str() << "' (entry " << i
          //              << "): got " << it->second << " expected 0.0");
          break;
        case 2:
          //   for (std::map<time_t, double>::const_iterator
          //          it(f.get_rras().front().begin()),
          //          end(f.get_rras().front().end());
          //        it != end;
          //        ++it)
          //     if (fabs(it->second) > 0.01)
          //       throw (exceptions::msg()
          //              << "invalid derive value in RRD file '"
          //              << path.str().c_str() << "' (entry " << i
          //              << "): got " << it->second << " expected 0.0");
          break;
        case 3:
          //   for (std::map<time_t, double>::const_iterator
          //          it(f.get_rras().front().begin()),
          //          end(f.get_rras().front().end());
          //        it != end;
          //        ++it)
          //     if (fabs(it->second - 135.25) > 0.01)
          //       throw (exceptions::msg()
          //              << "invalid gauge value in RRD file '"
          //              << path.str().c_str() << "' (entry " << i
          //              << "): got " << it->second << " expected 135.25");
          break;
        case 4:
          //   for (std::map<time_t, double>::const_iterator
          //          it(f.get_rras().front().begin()),
          //          end(f.get_rras().front().end());
          //        it != end;
          //        ++it)
          //     if (fabs(it->second - 9.0) > 0.01)
          //       throw (exceptions::msg()
          //              << "invalid default value in RRD file '"
          //              << path.str().c_str() << "' (entry " << i
          //              << "): got " << it->second << " expected 9.0");
          break;
      }
    }

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  config_remove(engine_config_path.c_str());
  ::remove(cbmod_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  recursive_remove(metrics_path);
  recursive_remove(status_path);

  return (retval);
}
