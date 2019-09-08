/*
** Copyright 2014-2015 Centreon
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

#include <QSqlError>
#include <QSqlQuery>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVariant>
#include <ctime>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define STORAGE_DB_NAME "broker_influxdb_centreon_storage"
#define COMMAND_FILE "broker_influxdb_command_file"

#define INFLUXDB_DB_NAME "influxdb_unittest"
#define INFLUXDB_DB_PORT "6421"
#define INFLUXDB_DB_PORT_S 6421
#define INFLUXDB_DB_PASSWORD "influxdb_password"
#define INFLUXDB_DB_HOST "localhost"
#define INFLUXDB_DB_USER "influxdb_user"

static const char* expected_result =
    "POST /write?u=" INFLUXDB_DB_USER "&p=" INFLUXDB_DB_PASSWORD
    " HTTP/1.0\n"
    "Content-Length: 442\n\n"
    "{\"database\":\"" INFLUXDB_DB_NAME
    "\",\"points\":["
    "{\"name\":\"status\",\"tags\":{\"status_id\":1 "
    "},\"timestamp\":$timestamp$,\"fields\":{\"value\":0,\"hostid\":\"1\","
    "\"host\":\"1\",\"serviceid\":\"1\",\"service\":\"1\",\"instanceid\":"
    "\"42\",\"instance\":\"MyBroker\" } },"
    "{\"name\":\"influxdb_test\",\"tags\":{\"metric_id\":1 "
    "},\"timestamp\":$timestamp$,\"fields\":{\"value\":0.8,\"metric\":"
    "\"influxdb_test\",\"hostid\":\"1\",\"host\":\"1\",\"serviceid\":\"1\","
    "\"service\":\"1\",\"instanceid\":\"42\",\"instance\":\"MyBroker\" } } ]}";

/**
 *  Check that the influxdb works.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  test_db db;
  engine_extcmd commander;
  engine monitoring;
  std::string engine_config_path(tmpnam(NULL));

  try {
    // Open the socket
    QTcpServer server;
    if (!server.listen(QHostAddress::Any, INFLUXDB_DB_PORT_S))
      throw exceptions::msg() << "couldn't listen to " << INFLUXDB_DB_PORT_S;

    // Prepare database.
    db.open(STORAGE_DB_NAME);

    // Create the config influxdb xml file.
    test_file file;
    file.set_template(PROJECT_SOURCE_DIR "/test/cfg/influxdb.xml.in");
    file.set("MYSQL_DB_NAME", STORAGE_DB_NAME);
    file.set("INFLUXDB_DB_NAME", INFLUXDB_DB_NAME);
    file.set("INFLUXDB_DB_PORT", INFLUXDB_DB_PORT);
    file.set("INFLUXDB_DB_PASSWORD", INFLUXDB_DB_PASSWORD);
    file.set("INFLUXDB_DB_HOST", INFLUXDB_DB_HOST);
    file.set("INFLUXDB_DB_USER", INFLUXDB_DB_USER);
    std::string config_file = file.generate();

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 1);
    generate_services(services, hosts, 1);
    services.back().accept_passive_service_checks = 1;
    services.back().checks_enabled = 0;
    services.back().max_attempts = 1;
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << config_file << "\n";
      additional_config = oss.str();
    }
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();

    sleep_for(3);
    time_t first_timestamp_possible = std::time(NULL);
    commander.execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;1;0;Submitted by unit test | "
        "influxdb_test=0.80");

    // Wait twice for an incoming connection. The first connection is
    // the connection used by influxdb module to check if the server exist,
    // the second one is the data.
    if (!server.waitForNewConnection(8000 * MONITORING_ENGINE_INTERVAL_LENGTH))
      throw exceptions::msg()
          << "no incoming connection to " << INFLUXDB_DB_PORT_S;
    QTcpSocket* s = server.nextPendingConnection();
    if (!s)
      throw exceptions::msg()
          << "no incoming connection to " << INFLUXDB_DB_PORT_S;
    delete s;
    if (!server.waitForNewConnection(8000 * MONITORING_ENGINE_INTERVAL_LENGTH))
      throw exceptions::msg()
          << "no incoming connection to " << INFLUXDB_DB_PORT_S;
    s = server.nextPendingConnection();
    if (!s)
      throw exceptions::msg()
          << "no incoming connection to " << INFLUXDB_DB_PORT_S;
    QByteArray array;
    while (s->isOpen() && s->waitForReadyRead())
      array.append(s->readAll());
    QString data(array);

    time_t last_timestamp_possible = std::time(NULL);
    bool got = false;
    // Check the data got for everything is okay.
    for (; first_timestamp_possible <= last_timestamp_possible;
         ++first_timestamp_possible) {
      QString expected = expected_result;
      expected.replace("$timestamp$",
                       QString::number(first_timestamp_possible));
      if (expected == data) {
        got = true;
        break;
      }
    }
    if (!got)
      throw(exceptions::msg() << "incorrect influxdb data: got: " << data
                              << "\nexpected: " << expected_result);
    delete s;

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  monitoring.stop();
  free_hosts(hosts);
  free_services(services);
  config_remove(engine_config_path.c_str());

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
