/*
** Copyright 2018 Centreon
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

#include <cmath>
#include <gtest/gtest.h>
#include <memory>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_check.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/instance_status.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/module.hh"
#include "com/centreon/broker/neb/host_group.hh"
#include "com/centreon/broker/neb/host_group_member.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_check.hh"
#include "com/centreon/broker/query_preparator.hh"

using namespace com::centreon::broker;

class DatabaseStorageTest : public ::testing::Test {
 public:
  void SetUp() {
    try {
      config::applier::init();
    }
    catch (std::exception const& e) {
      (void) e;
    }
  }
  void TearDown() {
    config::applier::deinit();
  }
};

// When there is no database
// Then the mysql creation throws an exception
TEST_F(DatabaseStorageTest, NoDatabase) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    9876,
    "admin",
    "centreon",
    "centreon_storage");
  std::auto_ptr<mysql> ms;
  ASSERT_THROW(ms.reset(new mysql(db_cfg)), exceptions::msg);
}

// When there is a database
// And when the connection is well done
// Then no exception is thrown and the mysql object is well built.
TEST_F(DatabaseStorageTest, ConnectionOk) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage");
  std::auto_ptr<mysql> ms;
  ASSERT_NO_THROW(ms.reset(new mysql(db_cfg)));
}

// Given a mysql object
// When an insert is done in database
// Then nothing is inserted before the commit.
// When the commit is done
// Then the insert is available in the database.
TEST_F(DatabaseStorageTest, SendDataBin) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream oss;
  int now(time(NULL));
  oss << "INSERT INTO data_bin (id_metric, ctime, status, value) VALUES "
      << "(1, " << now << ", '0', 2.5)";
  int thread_id(ms->run_query(oss.str()));
  oss.str("");
  oss << "SELECT id_metric, status FROM data_bin WHERE ctime=" << now;
  ms->run_query(
        oss.str(),
        "", false,
        thread_id);
  // The query is done from the same thread/connection
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_FALSE(ms->fetch_row(thread_id, res));
  ASSERT_NO_THROW(ms->commit(thread_id));

  thread_id = ms->run_query(oss.str());
  res = ms->get_result(thread_id);
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, PrepareQuery) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream oss;
  oss << "INSERT INTO " << "metrics"
      << "  (index_id, metric_name, unit_name, warn, warn_low,"
         "   warn_threshold_mode, crit, crit_low, "
         "   crit_threshold_mode, min, max, current_value,"
         "   data_source_type)"
         " VALUES (?, ?, ?, ?, "
         "         ?, ?, ?, "
         "         ?, ?, ?, ?, "
         "         ?, ?)";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream nss;
  nss << "metric_name - " << time(NULL);
  mysql_stmt stmt(ms->prepare_query(oss.str()));
  stmt.bind_value_as_i32(0, 19);
  stmt.bind_value_as_str(1, nss.str());
  stmt.bind_value_as_str(2, "test/s");
  stmt.bind_value_as_f32(3, NAN);
  stmt.bind_value_as_f32(4, INFINITY);
  stmt.bind_value_as_tiny(5, true);
  stmt.bind_value_as_f32(6, 10.0);
  stmt.bind_value_as_f32(7, 20.0);
  stmt.bind_value_as_tiny(8, false);
  stmt.bind_value_as_f32(9, 0.0);
  stmt.bind_value_as_f32(10, 50.0);
  stmt.bind_value_as_f32(11, 18.0);
  stmt.bind_value_as_str(12, "2");
  // We force the thread 0
  ms->run_statement(stmt, "", false, 0);
  oss.str("");
  oss << "SELECT metric_name FROM metrics WHERE metric_name='" << nss.str() << "'";
  int thread_id(ms->run_query(oss.str()));
  mysql_result res(ms->get_result(thread_id));
  ASSERT_FALSE(ms->fetch_row(thread_id, res));
  ASSERT_NO_THROW(ms->commit());
  thread_id = ms->run_query(oss.str());
  res = ms->get_result(thread_id);
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, PrepareQueryBadQuery) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream oss;
  oss << "INSERT INTO " << "metrics"
      << "  (index_id, metric_name, unit_name, warn, warn_low,"
         "   warn_threshold_mode, crit, crit_low, "
         "   crit_threshold_mode, min, max, current_value,"
         "   data_source_type)"
         " VALUES (?, ?, ?, ?, "
         "         ?, ?, ?, "
         "         ?, ?, ?, ?, "
         "         ?, ?";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream nss;
  nss << "metric_name - " << time(NULL);
  mysql_stmt stmt(ms->prepare_query(oss.str()));
  stmt.bind_value_as_i32(0, 19);
  stmt.bind_value_as_str(1, nss.str());
  stmt.bind_value_as_str(2, "test/s");
  stmt.bind_value_as_f32(3, NAN);
  stmt.bind_value_as_f32(4, INFINITY);
  stmt.bind_value_as_tiny(5, true);
  stmt.bind_value_as_f32(6, 10.0);
  stmt.bind_value_as_f32(7, 20.0);
  stmt.bind_value_as_tiny(8, false);
  stmt.bind_value_as_f32(9, 0.0);
  stmt.bind_value_as_f32(10, 50.0);
  stmt.bind_value_as_f32(11, 18.0);
  stmt.bind_value_as_str(12, "2");
  // The commit forces threads to empty their tasks stack
  ms->commit();
  // We are sure, the error is set.
  ASSERT_THROW(ms->run_statement(stmt, "", false, 0), std::exception);
}

TEST_F(DatabaseStorageTest, SelectSync) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::ostringstream oss;
  oss << "SELECT metric_id, index_id, metric_name FROM metrics LIMIT 10";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  int id(ms->run_query(oss.str()));
  mysql_result res(ms->get_result(id));
  int count(0);
  while (ms->fetch_row(id, res)) {
    int v(res.value_as_i32(0));
    std::string s(res.value_as_str(2));
    ASSERT_GT(v, 0);
    ASSERT_FALSE(s.empty());
    std::cout << "metric name " << v << " content: " << s << std::endl;
    ++count;
  }
  ASSERT_EQ(count, 10);
}

TEST_F(DatabaseStorageTest, QuerySyncWithError) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  int thread_id(ms->run_query("SELECT foo FROM bar LIMIT 1", "ERROR", true));
  ASSERT_THROW(ms->get_result(thread_id), exceptions::msg);
}

TEST_F(DatabaseStorageTest, QueryWithError) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  // The following insert fails
  ms->run_query("INSERT INTO FOO (toto) VALUES (0)", "", true, 1);
  ms->commit();

  // The following is the same one, executed by the same thread but since the
  // previous error, an exception should arrive.
  ASSERT_THROW(ms->run_query("INSERT INTO FOO (toto) VALUES (0)", "", true, 1), std::exception);
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, LastInsertId) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream nss;
  nss << "metric_name - " << time(NULL) << "bis";

  std::ostringstream oss;
  oss << "INSERT INTO metrics"
      << " (index_id, metric_name, unit_name, warn, warn_low,"
         " warn_threshold_mode, crit, crit_low,"
         " crit_threshold_mode, min, max, current_value,"
         " data_source_type)"
         " VALUES (19, '" << nss.str()
      << "', 'test/s', 20.0, 40.0, 1, 10.0, 20.0, 1, 0.0, 50.0, 18.0, '2')";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  // We force the thread 0
  std::cout << oss.str() << std::endl;
  int thread_id(ms->run_query(oss.str()));
  int id(ms->get_last_insert_id(thread_id));

  // Commit is needed to make the select later. But it is not needed to get
  // the id. Moreover, if we commit before getting the last id, the result will
  // be null.
  ms->commit();
  ASSERT_TRUE(id > 0);
  std::cout << "id = " << id << std::endl;
  oss.str("");
  oss << "SELECT metric_id FROM metrics WHERE metric_name = '"
    << nss.str() << "'";
  std::cout << oss.str() << std::endl;
  thread_id = ms->run_query(oss.str());
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_TRUE(res.value_as_i32(0) == id);
}

TEST_F(DatabaseStorageTest, PrepareQuerySync) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream oss;
  oss << "INSERT INTO metrics"
      << "  (index_id, metric_name, unit_name, warn, warn_low,"
         "   warn_threshold_mode, crit, crit_low, "
         "   crit_threshold_mode, min, max, current_value,"
         "   data_source_type)"
         " VALUES (?, ?, ?, ?, "
         "         ?, ?, ?, "
         "         ?, ?, ?, ?, "
         "         ?, ?)";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream nss;
  nss << "metric_name - " << time(NULL) << "bis2";
  mysql_stmt stmt(ms->prepare_query(oss.str()));
  stmt.bind_value_as_i32(0, 19);
  stmt.bind_value_as_str(1, nss.str());
  stmt.bind_value_as_str(2, "test/s");
  stmt.bind_value_as_f32(3, 20.0);
  stmt.bind_value_as_f32(4, 40.0);
  stmt.bind_value_as_tiny(5, 1);
  stmt.bind_value_as_f32(6, 10.0);
  stmt.bind_value_as_f32(7, 20.0);
  stmt.bind_value_as_tiny(8, 1);
  stmt.bind_value_as_f32(9, 0.0);
  stmt.bind_value_as_f32(10, 50.0);
  stmt.bind_value_as_f32(11, 18.0);
  stmt.bind_value_as_str(12, "2");
  // We force the thread 0
  int thread_id(ms->run_statement(stmt, "", 0));
  int id(ms->get_last_insert_id(thread_id));
  ASSERT_TRUE(id > 0);
  std::cout << "id = " << id << std::endl;
  oss.str("");
  oss << "SELECT metric_id FROM metrics WHERE metric_name='" << nss.str() << "'";
  thread_id = ms->run_query(oss.str());
  mysql_result res(ms->get_result(thread_id));
  ASSERT_FALSE(ms->fetch_row(thread_id, res));
  ASSERT_NO_THROW(ms->commit());
  thread_id = ms->run_query(oss.str());
  res = ms->get_result(thread_id);
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  std::cout << "id1 = " << res.value_as_i32(0) << std::endl;
  ASSERT_TRUE(res.value_as_i32(0) == id);
  ASSERT_TRUE(ms->get_affected_rows(thread_id) == 1);
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, RepeatPrepareQuery) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream oss;
  oss << "UPDATE metrics"
	 " SET unit_name=?, warn=?, warn_low=?, warn_threshold_mode=?,"
	 " crit=?, crit_low=?, crit_threshold_mode=?,"
	 " min=?, max=?, current_value=? "
	 "WHERE metric_id=?";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  mysql_stmt stmt(ms->prepare_query(oss.str()));
  for (int i(1); i < 4000; ++i) {
    stmt.bind_value_as_str(0, "test/s");
    stmt.bind_value_as_f32(1, NAN);
    stmt.bind_value_as_f32(2, NAN);
    stmt.bind_value_as_tiny(3, 0);
    stmt.bind_value_as_f32(4, NAN);
    stmt.bind_value_as_f32(5, NAN);
    stmt.bind_value_as_tiny(6, 0);
    stmt.bind_value_as_f32(7, 10.0);
    stmt.bind_value_as_f32(8, 20.0);
    stmt.bind_value_as_f32(9, 18.0);
    stmt.bind_value_as_i32(10, i);

    ms->run_statement(stmt);
  }
  ms->commit();
}

// Instance (15) statement
TEST_F(DatabaseStorageTest, InstanceStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("instance_id");
  query_preparator qp(neb::instance::static_type(), unique);
  mysql_stmt inst_insupdate(qp.prepare_insert_or_update(*ms));
  mysql_stmt inst_delete(qp.prepare_delete(*ms));

  neb::instance inst;
  inst.poller_id = 1;
  inst.name = "Central";
  inst.program_start = time(NULL) - 100;
  inst.program_end = time(NULL) - 1;
  inst.version = "1.8.1";

  inst_insupdate << inst;
  ms->run_statement(inst_insupdate, "", false, 0);

  // Deletion
  inst_delete << inst;
  ms->run_statement(inst_delete, "", false, 0);

  // Insert
  inst_insupdate << inst;
  ms->run_statement(inst_insupdate, "", false, 0);

  // Update
  inst.program_end = time(NULL);
  inst_insupdate << inst;
  ms->run_statement(inst_insupdate, "", false, 0);

  ms->commit();

  std::stringstream oss;
  oss << "SELECT instance_id FROM instances WHERE "
    "instance_id=1";
  int thread_id(ms->run_query(oss.str()));
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
}

// Host (12) statement
TEST_F(DatabaseStorageTest, HostStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));

  ms->run_query("DELETE FROM hosts");
  query_preparator::event_unique unique;
  unique.insert("host_id");
  query_preparator qp(neb::host::static_type(), unique);
  mysql_stmt host_insupdate(qp.prepare_insert_or_update(*ms));

  neb::host h;
  h.host_id = 24;
  h.address = "10.0.2.15";
  h.alias = "central";
  h.flap_detection_on_down = true;
  h.flap_detection_on_unreachable = true;
  h.flap_detection_on_up = true;
  h.host_name = "central_9";
  h.notify_on_down = true;
  h.notify_on_unreachable = true;
  h.poller_id = 1;
  h.stalk_on_down = false;
  h.stalk_on_unreachable = false;
  h.stalk_on_up = false;
  h.statusmap_image = "";
  h.timezone = "Europe/Paris";

  int thread_id(ms->run_query("DELETE FROM hosts"));
  ms->get_affected_rows(thread_id);

  // Insert
  host_insupdate << h;
  ms->run_statement(host_insupdate, "", false, 0);

  // Update
  h.stalk_on_up = true;
  host_insupdate << h;
  ms->run_statement(host_insupdate, "", false, 0);

  ms->commit();

  thread_id = ms->run_query(
        "SELECT stalk_on_up FROM hosts WHERE "
        "host_id=24");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_TRUE(res.value_as_bool(0));
}

TEST_F(DatabaseStorageTest, CustomVarStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("name");
  unique.insert("service_id");
  query_preparator qp(neb::custom_variable::static_type(), unique);
  mysql_stmt cv_insert_or_update(qp.prepare_insert_or_update(*ms));
  mysql_stmt cv_delete(qp.prepare_delete(*ms));

  neb::custom_variable cv;
  cv.service_id = 498;
  cv.update_time = time(NULL);
  cv.modified = false;
  cv.host_id = 31;
  cv.name = "PROCESSNAME";
  cv.value = "centengine";
  cv.default_value = "centengine";

  cv_insert_or_update << cv;
  ms->run_statement(cv_insert_or_update, "", false, 0);

  // Deletion
  cv_delete << cv;
  ms->run_statement(cv_delete, "", false, 0);

  // Insert
  cv_insert_or_update << cv;
  ms->run_statement(cv_insert_or_update, "", false, 0);

  // Update
  cv.update_time = time(NULL) + 1;
  cv_insert_or_update << cv;
  ms->run_statement(cv_insert_or_update, "", false, 0);

  ms->commit();

  std::stringstream oss;
  oss << "SELECT host_id FROM customvariables WHERE "
    "host_id=31 AND service_id=498"
    " AND name='PROCESSNAME'";
  int thread_id(ms->run_query(oss.str()));
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_FALSE(ms->fetch_row(thread_id, res));
}

TEST_F(DatabaseStorageTest, ModuleStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator qp(neb::module::static_type());
  mysql_stmt module_insert(qp.prepare_insert(*ms));

  neb::module m;
  m.should_be_loaded = true;
  m.filename = "/usr/lib64/nagios/cbmod.so";
  m.loaded = true;
  m.poller_id = 1;
  m.args = "/etc/centreon-broker/central-module.xml";

  // Deletion
  int thread_id(ms->run_query("DELETE FROM modules"));
  ms->get_affected_rows(thread_id);

  // Insert
  module_insert << m;
  ms->run_statement(module_insert, "", false);
  ms->commit();

  thread_id = ms->run_query("SELECT module_id FROM modules LIMIT 1");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
}

// log_entry (17) statement queries
TEST_F(DatabaseStorageTest, LogStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator qp(neb::log_entry::static_type());
  mysql_stmt log_insert(qp.prepare_insert(*ms));

  neb::log_entry le;
  le.poller_name = "Central";
  le.msg_type = 5;
  le.output = "Event loop start at bar date";
  le.notification_contact = "";
  le.notification_cmd = "";
  le.status = 0;
  le.host_name = "";
  le.c_time = time(NULL);

  // Deletion
  int thread_id(ms->run_query("DELETE FROM logs"));
  ms->get_affected_rows(thread_id);

  // Insert
  log_insert << le;
  ms->run_statement(log_insert, "", false);
  ms->commit();

  thread_id = ms->run_query(
        "SELECT log_id FROM logs "
        "WHERE output = \"Event loop start at bar date\"");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
}

// Instance status (16) statement
TEST_F(DatabaseStorageTest, InstanceStatusStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("instance_id");
  query_preparator qp(neb::instance_status::static_type(), unique);
  mysql_stmt inst_status_update(qp.prepare_update(*ms));

  neb::instance_status is;
  is.active_host_checks_enabled = true;
  is.active_service_checks_enabled = true;
  is.check_hosts_freshness = false;
  is.check_services_freshness = true;
  is.global_host_event_handler = "";
  is.global_service_event_handler = "";
  is.last_alive = time(NULL) - 5;
  is.obsess_over_hosts = false;
  is.obsess_over_services = false;
  is.passive_host_checks_enabled = true;
  is.passive_service_checks_enabled = true;
  is.poller_id = 1;

  // Insert
  inst_status_update << is;
  int thread_id(ms->run_statement(inst_status_update, "", false));
  ASSERT_TRUE(ms->get_affected_rows(thread_id, inst_status_update) == 1);
  ms->commit();

  thread_id = ms->run_query(
        "SELECT active_host_checks FROM instances "
        "WHERE instance_id=1");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_TRUE(res.value_as_bool(0));
}

// Host check (8) statement
TEST_F(DatabaseStorageTest, HostCheckStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  query_preparator qp(neb::host_check::static_type(), unique);
  mysql_stmt host_check_update(qp.prepare_update(*ms));

  neb::host_check hc;
  hc.command_line = "/usr/lib/nagios/plugins/check_icmp -H 10.0.2.15 -w 3000.0,80% -c 5000.0,100% -p 1";
  hc.host_id = 24;

  // Update
  host_check_update << hc;
  ms->run_statement(host_check_update, "", false);
  ms->commit();

  int thread_id(ms->run_query(
        "SELECT host_id FROM hosts WHERE host_id=24"));
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
}

// Host status (14) statement
TEST_F(DatabaseStorageTest, HostStatusStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  query_preparator qp(neb::host_status::static_type(), unique);
  mysql_stmt host_status_update(qp.prepare_update(*ms));

  neb::host_status hs;
  hs.active_checks_enabled = true;
  hs.check_command = "base_host_alive";
  hs.check_interval = 5;
  hs.check_period = "24x7";
  hs.check_type = 0;
  hs.current_check_attempt = 1;
  hs.current_state = 0;
  hs.downtime_depth = 0;
  hs.enabled = true;
  hs.execution_time = 0.159834;
  hs.has_been_checked = true;
  hs.host_id = 24;
  hs.last_check = time(NULL) - 3;
  hs.last_hard_state = 0;
  hs.last_update = time(NULL) - 300;
  hs.latency = 0.001;
  hs.max_check_attempts = 3;
  hs.next_check = time(NULL) + 50;
  hs.obsess_over = true;
  hs.output = "OK - 10.0.2.15: rta 0,020ms, lost 0%\n";
  hs.perf_data = "rta=0,020ms;3000,000;5000,000;0; pl=0%;80;100;; rtmax=0,020ms;;;; rtmin=0,020ms;;;;";
  hs.retry_interval = 1;
  hs.should_be_scheduled = true;
  hs.state_type = 1;

  // Update
  host_status_update << hs;
  int thread_id(ms->run_statement(host_status_update, "", false));

  ASSERT_TRUE(ms->get_affected_rows(thread_id, host_status_update) == 1);

  ms->commit();
  thread_id = ms->run_query(
                    "SELECT execution_time FROM hosts WHERE host_id=24");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_TRUE(res.value_as_f64(0) == 0.159834);
}

// Service (23) statement
TEST_F(DatabaseStorageTest, ServiceStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("service_id");
  query_preparator qp(neb::service::static_type(), unique);
  mysql_stmt service_insupdate(qp.prepare_insert_or_update(*ms));

  int thread_id(ms->run_query("DELETE FROM services"));
  ms->get_affected_rows(thread_id);

  neb::service s;
  s.host_id = 24;
  s.service_id = 318;
  s.default_active_checks_enabled = true;
  s.default_event_handler_enabled = true;
  s.default_flap_detection_enabled = true;
  s.default_notifications_enabled = true;
  s.default_passive_checks_enabled = true;
  s.display_name = "test-dbr";
  s.icon_image = "";
  s.icon_image_alt = "";
  s.notification_interval = 30;
  s.notification_period = "";
  s.notify_on_downtime = true;
  s.notify_on_flapping = true;
  s.notify_on_recovery = true;
  s.retain_nonstatus_information = true;
  s.retain_status_information = true;

  // Update
  service_insupdate << s;
  thread_id = ms->run_statement(service_insupdate, "", false);

  ms->commit();
  thread_id = ms->run_query(
                    "SELECT notification_interval FROM services WHERE host_id=24 AND service_id=318");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_TRUE(res.value_as_i32(0) == 30);
}

// Service Check (19) statement
TEST_F(DatabaseStorageTest, ServiceCheckStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("service_id");
  query_preparator qp(neb::service_check::static_type(), unique);
  mysql_stmt service_check_update(qp.prepare_update(*ms));

  neb::service_check sc;
  sc.service_id = 318;
  sc.host_id = 24;
  sc.command_line = "/usr/bin/bash /home/admin/test.sh";

  // Update
  service_check_update << sc;
  int thread_id(ms->run_statement(service_check_update, "", false));

  ASSERT_TRUE(ms->get_affected_rows(thread_id, service_check_update) == 1);

  ms->commit();
  thread_id = ms->run_query(
                    "SELECT command_line FROM services WHERE host_id=24 AND service_id=318");
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(ms->fetch_row(thread_id, res));
  ASSERT_TRUE(res.value_as_str(0) == "/usr/bin/bash /home/admin/test.sh");
}

// Service Status (24) statement
TEST_F(DatabaseStorageTest, ServiceStatusStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("service_id");
  query_preparator qp(neb::service_status::static_type(), unique);
  mysql_stmt service_status_update(qp.prepare_update(*ms));

  neb::service_status ss;
  ss.last_time_critical = time(NULL) - 1000;
  ss.last_time_ok = time(NULL) - 50;
  ss.last_time_unknown = time(NULL) - 1500;
  ss.last_time_warning = time(NULL) - 500;
  ss.service_id = 318;
  ss.host_id = 24;

  // Update
  service_status_update << ss;
  int thread_id(ms->run_statement(service_status_update, "", false));

  ASSERT_TRUE(ms->get_affected_rows(thread_id, service_status_update) == 1);

  ms->commit();
}

TEST_F(DatabaseStorageTest, SelectStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  std::string query("SELECT value,status FROM data_bin WHERE ctime >= ?");
  mysql_stmt select_stmt(ms->prepare_query(query));
  select_stmt.bind_value_as_u64(0, time(NULL) - 20);
  int thread_id(ms->run_statement(select_stmt));
  mysql_result res(ms->get_result(thread_id, select_stmt));

  while (ms->fetch_row(thread_id, res)) {
    ASSERT_TRUE(res.value_as_f64(0) == 2.5);
    ASSERT_TRUE(res.value_as_i32(1) == 0);
  }
}

TEST_F(DatabaseStorageTest, DowntimeStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));

  std::ostringstream oss;
  oss << "INSERT INTO " << ((ms->schema_version() == mysql::v2)
                            ? "downtimes"
                            : "rt_downtimes")
      << " (actual_end_time, "
         "actual_start_time, "
         "author, type, deletion_time, duration, end_time, entry_time, "
         "fixed, host_id, instance_id, internal_id, service_id, "
         "start_time, triggered_by, cancelled, started, comment_data) "
         "VALUES(:actual_end_time,:actual_start_time,:author,:type,:deletion_time,:duration,:end_time,:entry_time,:fixed,:host_id,:instance_id,:internal_id,:service_id,:start_time,:triggered_by,:cancelled,:started,:comment_data) ON DUPLICATE KEY UPDATE "
         "actual_end_time=GREATEST(COALESCE(actual_end_time, -1), :actual_end_time),"
         "actual_start_time=COALESCE(actual_start_time, :actual_start_time),"
         "author=:author, cancelled=:cancelled, comment_data=:comment_data,"
         "deletion_time=:deletion_time, duration=:duration, end_time=:end_time,"
         "fixed=:fixed, host_id=:host_id, service_id=:service_id,"
         "start_time=:start_time, started=:started,"
         "triggered_by=:triggered_by, type=:type";
  mysql_stmt downtime_insupdate(mysql_stmt(oss.str(), true));
  ms->prepare_statement(downtime_insupdate);

  time_t now(time(NULL));

  neb::downtime d;
  d.actual_end_time = now;
  d.actual_start_time = now - 30;
  d.comment = "downtime test";
  d.downtime_type = 1;
  d.duration = 30;
  d.end_time = now;
  d.entry_time = now - 30;
  d.fixed = true;
  d.host_id = 24;
  d.poller_id = 1;
  d.service_id = 318;
  d.start_time = now - 30;
  d.was_started = true;

  downtime_insupdate << d;
  int thread_id(ms->run_statement(downtime_insupdate));

  std::cout << "downtime_insupdate: " << downtime_insupdate.get_query() << std::endl;

  ASSERT_TRUE(ms->get_affected_rows(thread_id, downtime_insupdate) == 1);
  ms->commit();
}

TEST_F(DatabaseStorageTest, HostGroupMemberStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "root",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));

  ms->run_query("DELETE FROM hostgroups");
  ms->run_query("DELETE FROM hosts_hostgroups");
  ms->commit();

  query_preparator::event_unique unique;
  unique.insert("hostgroup_id");
  unique.insert("host_id");
  query_preparator
    qp(neb::host_group_member::static_type(), unique);
  mysql_stmt host_group_member_insert(qp.prepare_insert(*ms));

  query_preparator::event_unique unique1;
  unique1.insert("hostgroup_id");
  query_preparator
    qp1(neb::host_group::static_type(), unique1);
  mysql_stmt host_group_insupdate(qp1.prepare_insert_or_update(*ms));

  neb::host_group_member hgm;
  hgm.enabled = true;
  hgm.group_id = 8;
  hgm.group_name = "Test host group";
  hgm.host_id = 24;
  hgm.poller_id = 1;

  host_group_member_insert << hgm;
  std::cout << host_group_member_insert.get_query() << std::endl;

  int thread_id(ms->run_statement(
                      host_group_member_insert,
                      "Error: host group not defined", true));

  neb::host_group hg;
  hg.id = 8;
  hg.name = "Test hostgroup";
  hg.enabled = true;
  hg.poller_id = 1;

  host_group_insupdate << hg;

  std::cout << host_group_insupdate.get_query() << std::endl;

  ms->run_statement_on_condition(
                 host_group_insupdate, mysql_task::ON_ERROR,
                 "Error: Unable to create host group", true,
                 thread_id);

  ms->run_statement_on_condition(
                 host_group_member_insert, mysql_task::IF_PREVIOUS,
                 "Error: host group not defined", true,
                 thread_id);
  ms->commit();
}
