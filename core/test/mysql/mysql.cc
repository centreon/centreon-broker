/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include "com/centreon/broker/mysql.hh"

#include <gtest/gtest.h>

#include <cmath>
#include <future>
#include <memory>

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_check.hh"
#include "com/centreon/broker/neb/host_group.hh"
#include "com/centreon/broker/neb/host_group_member.hh"
#include "com/centreon/broker/neb/host_parent.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/instance_status.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/module.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_check.hh"
#include "com/centreon/broker/neb/service_group.hh"
#include "com/centreon/broker/neb/service_group_member.hh"
#include "com/centreon/broker/query_preparator.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

class DatabaseStorageTest : public ::testing::Test {
 public:
  void SetUp() override {
    try {
      config::applier::init();
    } catch (std::exception const& e) {
      (void)e;
    }
  }
  void TearDown() override { config::applier::deinit(); }
};

// When there is no database
// Then the mysql creation throws an exception
TEST_F(DatabaseStorageTest, NoDatabase) {
  database_config db_cfg("MySQL", "127.0.0.1", 9876, "admin", "centreon",
                         "centreon_storage");
  std::unique_ptr<mysql> ms;
  ASSERT_THROW(ms.reset(new mysql(db_cfg)), exceptions::msg);
}

// When there is a database
// And when the connection is well done
// Then no exception is thrown and the mysql object is well built.
TEST_F(DatabaseStorageTest, ConnectionOk) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage");
  std::unique_ptr<mysql> ms;
  ASSERT_NO_THROW(ms.reset(new mysql(db_cfg)));
}

// Given a mysql object
// When an insert is done in database
// Then nothing is inserted before the commit.
// When the commit is done
// Then the insert is available in the database.
TEST_F(DatabaseStorageTest, SendDataBin) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream oss;
  int now(time(nullptr));
  oss << "INSERT INTO data_bin (id_metric, ctime, status, value) VALUES "
      << "(1, " << now << ", '0', 2.5)";
  int thread_id(ms->run_query(oss.str(), "PROBLEME", true));
  oss.str("");
  oss << "SELECT id_metric, status FROM data_bin WHERE ctime=" << now;
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result(oss.str(), &promise, thread_id);

  // The query is done from the same thread/connection
  mysql_result res(promise.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_FALSE(ms->fetch_row(res));
  ASSERT_NO_THROW(ms->commit(thread_id));

  promise = std::promise<mysql_result>();
  ms->run_query_and_get_result(oss.str(), &promise, thread_id);
  mysql_result res1(promise.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res1));
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, PrepareQuery) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::ostringstream oss;
  oss << "INSERT INTO "
      << "metrics"
      << "  (index_id, metric_name, unit_name, warn, warn_low,"
         "   warn_threshold_mode, crit, crit_low, "
         "   crit_threshold_mode, min, max, current_value,"
         "   data_source_type)"
         " VALUES (?, ?, ?, ?, "
         "         ?, ?, ?, "
         "         ?, ?, ?, ?, "
         "         ?, ?)";

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream nss;
  nss << "metric_name - " << time(nullptr);
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
  oss << "SELECT metric_name FROM metrics WHERE metric_name='" << nss.str()
      << "'";
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result(oss.str(), &promise);
  mysql_result res(promise.get_future().get());
  ASSERT_FALSE(ms->fetch_row(res));
  ASSERT_NO_THROW(ms->commit());
  promise = std::promise<mysql_result>();
  ms->run_query_and_get_result(oss.str(), &promise);
  res = promise.get_future().get();
  ASSERT_TRUE(ms->fetch_row(res));
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, PrepareQueryBadQuery) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::ostringstream oss;
  oss << "INSERT INTO "
      << "metrics"
      << "  (index_id, metric_name, unit_name, warn, warn_low,"
         "   warn_threshold_mode, crit, crit_low, "
         "   crit_threshold_mode, min, max, current_value,"
         "   data_source_type)"
         " VALUES (?, ?, ?, ?, "
         "         ?, ?, ?, "
         "         ?, ?, ?, ?, "
         "         ?, ?";

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream nss;
  nss << "metric_name - " << time(nullptr);
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
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::ostringstream oss;
  oss << "SELECT metric_id, index_id, metric_name FROM metrics LIMIT 10";

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result(oss.str(), &promise);
  mysql_result res(promise.get_future().get());
  int count(0);
  while (ms->fetch_row(res)) {
    int v(res.value_as_i32(0));
    std::string s(res.value_as_str(2));
    ASSERT_GT(v, 0);
    ASSERT_FALSE(s.empty());
    std::cout << "metric name " << v << " content: " << s << std::endl;
    ++count;
  }
  ASSERT_TRUE(count > 0 && count <= 10);
}

TEST_F(DatabaseStorageTest, QuerySyncWithError) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result("SELECT foo FROM bar LIMIT 1", &promise);
  ASSERT_THROW(promise.get_future().get(), exceptions::msg);
}

TEST_F(DatabaseStorageTest, QueryWithError) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  // The following insert fails
  ms->run_query("INSERT INTO FOO (toto) VALUES (0)", "", true, 1);
  ms->commit();

  // The following is the same one, executed by the same thread but since the
  // previous error, an exception should arrive.
  ASSERT_THROW(ms->run_query("INSERT INTO FOO (toto) VALUES (0)", "", true, 1),
               std::exception);
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, LastInsertId) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::ostringstream nss;
  nss << "metric_name - " << time(nullptr) << "bis";

  std::ostringstream oss;
  oss << "INSERT INTO metrics"
      << " (index_id, metric_name, unit_name, warn, warn_low,"
         " warn_threshold_mode, crit, crit_low,"
         " crit_threshold_mode, min, max, current_value,"
         " data_source_type)"
         " VALUES (19, '"
      << nss.str()
      << "', 'test/s', 20.0, 40.0, 1, 10.0, 20.0, 1, 0.0, 50.0, 18.0, '2')";

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  // We force the thread 0
  std::cout << oss.str() << std::endl;
  std::promise<int> promise;
  ms->run_query_and_get_int(oss.str(), &promise,
                            mysql_task::int_type::LAST_INSERT_ID);
  int id(promise.get_future().get());

  // Commit is needed to make the select later. But it is not needed to get
  // the id. Moreover, if we commit before getting the last id, the result will
  // be null.
  ms->commit();
  ASSERT_TRUE(id > 0);
  std::cout << "id = " << id << std::endl;
  oss.str("");
  oss << "SELECT metric_id FROM metrics WHERE metric_name = '" << nss.str()
      << "'";
  std::cout << oss.str() << std::endl;
  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(oss.str(), &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_i32(0) == id);
}

TEST_F(DatabaseStorageTest, PrepareQuerySync) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
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

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream nss;
  nss << "metric_name - " << time(nullptr) << "bis2";
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
  std::promise<int> promise;
  ms->run_statement_and_get_int(stmt, &promise, mysql_task::LAST_INSERT_ID, 0);
  int id(promise.get_future().get());
  ASSERT_TRUE(id > 0);
  std::cout << "id = " << id << std::endl;
  oss.str("");
  oss << "SELECT metric_id FROM metrics WHERE metric_name='" << nss.str()
      << "'";
  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(oss.str(), &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_FALSE(ms->fetch_row(res));
  ASSERT_NO_THROW(ms->commit());
  promise_r = std::promise<mysql_result>();
  ms->run_query_and_get_result(oss.str(), &promise_r);
  res = promise_r.get_future().get();
  ASSERT_TRUE(ms->fetch_row(res));
  std::cout << "id1 = " << res.value_as_i32(0) << std::endl;
  ASSERT_TRUE(res.value_as_i32(0) == id);
}

// Given a mysql object
// When a prepare statement is done
// Then we can bind values to it and execute the statement.
// Then a commit makes data available in the database.
TEST_F(DatabaseStorageTest, RepeatPrepareQuery) {
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::ostringstream oss;
  oss << "UPDATE metrics"
         " SET unit_name=?, warn=?, warn_low=?, warn_threshold_mode=?,"
         " crit=?, crit_low=?, crit_threshold_mode=?,"
         " min=?, max=?, current_value=? "
         "WHERE metric_id=?";

  std::unique_ptr<mysql> ms(new mysql(db_cfg));
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
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("instance_id");
  query_preparator qp(neb::instance::static_type(), unique);
  mysql_stmt inst_insupdate(qp.prepare_insert_or_update(*ms));
  mysql_stmt inst_delete(qp.prepare_delete(*ms));

  neb::instance inst;
  inst.poller_id = 1;
  inst.name = "Central";
  inst.program_start = time(nullptr) - 100;
  inst.program_end = time(nullptr) - 1;
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
  inst.program_end = time(nullptr);
  inst_insupdate << inst;
  ms->run_statement(inst_insupdate, "", false, 0);

  // Second instance
  inst.poller_id = 2;
  inst.name = "Central2";
  inst_insupdate << inst;
  ms->run_statement(inst_insupdate, "", false, 0);

  ms->commit();

  std::stringstream oss;
  oss << "SELECT instance_id FROM instances ORDER BY instance_id";
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result(oss.str(), &promise);
  mysql_result res(promise.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_i32(0) == 1);
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_i32(0) == 2);
}

// Host (12) statement
TEST_F(DatabaseStorageTest, HostStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));

  query_preparator::event_unique unique;
  unique.insert("host_id");
  query_preparator qp(neb::host::static_type(), unique);
  mysql_stmt host_insupdate(qp.prepare_insert_or_update(*ms));

  neb::host h;
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

  std::promise<int> promise;
  ms->run_query_and_get_int("DELETE FROM hosts", &promise,
                            mysql_task::int_type::AFFECTED_ROWS);
  // We wait for the insertion.
  promise.get_future().get();

  // Insert
  for (int i(0); i < 50; ++i) {
    h.host_id = 24 + i;
    host_insupdate << h;
    ms->run_statement(host_insupdate, "", false, i % 5);
  }

  // Update
  for (int i(0); i < 50; ++i) {
    h.host_id = 24 + i;
    h.stalk_on_up = true;
    host_insupdate << h;
    ms->run_statement(host_insupdate, "", false, i % 5);
  }

  ms->commit();

  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(
      "SELECT stalk_on_up FROM hosts WHERE "
      "host_id=24",
      &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_bool(0));

  h.host_id = 1;
  h.address = "10.0.2.16";
  h.alias = "central1";
  h.flap_detection_on_down = true;
  h.flap_detection_on_unreachable = true;
  h.flap_detection_on_up = true;
  h.host_name = "central_1";
  h.notify_on_down = true;
  h.notify_on_unreachable = true;
  h.poller_id = 1;
  h.stalk_on_down = false;
  h.stalk_on_unreachable = false;
  h.stalk_on_up = false;
  h.statusmap_image = "";
  h.timezone = "Europe/Paris";

  host_insupdate << h;
  ms->run_statement(host_insupdate, "", false, 0);
  ms->commit();
  promise_r = std::promise<mysql_result>();
  ms->run_query_and_get_result("SELECT host_id FROM hosts", &promise_r);
  res = promise_r.get_future().get();
  for (int i = 0; i < 2; ++i) {
    ASSERT_TRUE(ms->fetch_row(res));
    int v(res.value_as_i32(0));
    ASSERT_TRUE(v == 1 || v == 24);
  }
}

TEST_F(DatabaseStorageTest, CustomVarStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("name");
  unique.insert("service_id");
  query_preparator qp(neb::custom_variable::static_type(), unique);
  mysql_stmt cv_insert_or_update(qp.prepare_insert_or_update(*ms));
  mysql_stmt cv_delete(qp.prepare_delete(*ms));

  neb::custom_variable cv;
  cv.service_id = 498;
  cv.update_time = time(nullptr);
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
  cv.update_time = time(nullptr) + 1;
  cv_insert_or_update << cv;
  ms->run_statement(cv_insert_or_update, "", false, 0);

  ms->commit();

  std::stringstream oss;
  oss << "SELECT host_id FROM customvariables WHERE "
         "host_id=31 AND service_id=498"
         " AND name='PROCESSNAME'";
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result(oss.str(), &promise);
  mysql_result res(promise.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_FALSE(ms->fetch_row(res));
}

TEST_F(DatabaseStorageTest, ModuleStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator qp(neb::module::static_type());
  mysql_stmt module_insert(qp.prepare_insert(*ms));

  neb::module m;
  m.should_be_loaded = true;
  m.filename = "/usr/lib64/nagios/cbmod.so";
  m.loaded = true;
  m.poller_id = 1;
  m.args = "/etc/centreon-broker/central-module.xml";

  // Deletion
  std::promise<int> promise;
  ms->run_query_and_get_int("DELETE FROM modules", &promise,
                            mysql_task::AFFECTED_ROWS);
  // We wait for the deletion to be done
  promise.get_future().get();

  // Insert
  module_insert << m;
  ms->run_statement(module_insert, "", false);
  ms->commit();

  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result("SELECT module_id FROM modules LIMIT 1",
                               &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
}

// log_entry (17) statement queries
TEST_F(DatabaseStorageTest, LogStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
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
  le.c_time = time(nullptr);

  // Deletion
  std::promise<int> promise;
  ms->run_query_and_get_int("DELETE FROM logs", &promise,
                            mysql_task::int_type::AFFECTED_ROWS);
  // We wait for the deletion
  promise.get_future().get();

  // Insert
  log_insert << le;
  ms->run_statement(log_insert, "", false);
  ms->commit();

  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(
      "SELECT log_id FROM logs "
      "WHERE output = \"Event loop start at bar date\"",
      &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
}

// Instance status (16) statement
TEST_F(DatabaseStorageTest, InstanceStatusStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
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
  is.last_alive = time(nullptr) - 5;
  is.obsess_over_hosts = false;
  is.obsess_over_services = false;
  is.passive_host_checks_enabled = true;
  is.passive_service_checks_enabled = true;
  is.poller_id = 1;

  // Insert
  inst_status_update << is;
  std::promise<int> promise;
  ms->run_statement_and_get_int(inst_status_update, &promise,
                                mysql_task::int_type::AFFECTED_ROWS);
  ASSERT_TRUE(promise.get_future().get() == 1);
  ms->commit();

  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(
      "SELECT active_host_checks FROM instances "
      "WHERE instance_id=1",
      &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_bool(0));
}

// Host check (8) statement
TEST_F(DatabaseStorageTest, HostCheckStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  query_preparator qp(neb::host_check::static_type(), unique);
  mysql_stmt host_check_update(qp.prepare_update(*ms));

  neb::host_check hc;
  hc.command_line =
      "/usr/lib/nagios/plugins/check_icmp -H 10.0.2.15 -w 3000.0,80% -c "
      "5000.0,100% -p 1";
  hc.host_id = 24;

  // Update
  host_check_update << hc;
  ms->run_statement(host_check_update, "", false);
  ms->commit();

  std::promise<mysql_result> promise;
  ms->run_query_and_get_result("SELECT host_id FROM hosts WHERE host_id=24",
                               &promise);
  mysql_result res(promise.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
}

// Host status (14) statement
TEST_F(DatabaseStorageTest, HostStatusStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
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
  hs.last_check = time(nullptr) - 3;
  hs.last_hard_state = 0;
  hs.last_update = time(nullptr) - 300;
  hs.latency = 0.001;
  hs.max_check_attempts = 3;
  hs.next_check = time(nullptr) + 50;
  hs.obsess_over = true;
  hs.output = "OK - 10.0.2.15: rta 0,020ms, lost 0%\n";
  hs.perf_data =
      "rta=0,020ms;3000,000;5000,000;0; pl=0%;80;100;; rtmax=0,020ms;;;; "
      "rtmin=0,020ms;;;;";
  hs.retry_interval = 1;
  hs.should_be_scheduled = true;
  hs.state_type = 1;

  // Update
  host_status_update << hs;
  std::promise<int> promise;
  ms->run_statement_and_get_int(host_status_update, &promise,
                                mysql_task::int_type::AFFECTED_ROWS);

  ASSERT_TRUE(promise.get_future().get() == 1);

  ms->commit();
  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(
      "SELECT execution_time FROM hosts WHERE host_id=24", &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_f64(0) == 0.159834);
}

// Service (23) statement
TEST_F(DatabaseStorageTest, ServiceStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("service_id");
  query_preparator qp(neb::service::static_type(), unique);
  mysql_stmt service_insupdate(qp.prepare_insert_or_update(*ms));

  std::promise<int> promise;
  ms->run_query_and_get_int("DELETE FROM services", &promise,
                            mysql_task::int_type::AFFECTED_ROWS);
  promise.get_future().get();

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
  ms->run_statement(service_insupdate, "", false);

  ms->commit();
  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(
      "SELECT notification_interval FROM services WHERE host_id=24 AND "
      "service_id=318",
      &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_i32(0) == 30);
}

// Service Check (19) statement
TEST_F(DatabaseStorageTest, ServiceCheckStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
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
  std::promise<int> promise;
  ms->run_statement_and_get_int(service_check_update, &promise,
                                mysql_task::int_type::AFFECTED_ROWS);

  ASSERT_TRUE(promise.get_future().get() == 1);

  ms->commit();
  std::promise<mysql_result> promise_r;
  ms->run_query_and_get_result(
      "SELECT command_line FROM services WHERE host_id=24 AND service_id=318",
      &promise_r);
  mysql_result res(promise_r.get_future().get());
  ASSERT_TRUE(ms->fetch_row(res));
  ASSERT_TRUE(res.value_as_str(0) == "/usr/bin/bash /home/admin/test.sh");
}

// Service Status (24) statement
TEST_F(DatabaseStorageTest, ServiceStatusStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("service_id");
  query_preparator qp(neb::service_status::static_type(), unique);
  mysql_stmt service_status_update(qp.prepare_update(*ms));

  neb::service_status ss;
  ss.last_time_critical = time(nullptr) - 1000;
  ss.last_time_ok = time(nullptr) - 50;
  ss.last_time_unknown = time(nullptr) - 1500;
  ss.last_time_warning = time(nullptr) - 500;
  ss.service_id = 318;
  ss.host_id = 24;

  // Update
  service_status_update << ss;
  std::promise<int> promise;
  ms->run_statement_and_get_int(service_status_update, &promise,
                                mysql_task::int_type::AFFECTED_ROWS);

  ASSERT_TRUE(promise.get_future().get() == 1);

  ms->commit();
}

TEST_F(DatabaseStorageTest, CustomvariableStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));

  query_preparator::event_unique unique;
  unique.insert("host_id");
  unique.insert("name");
  unique.insert("service_id");
  query_preparator qp(neb::custom_variable::static_type(), unique);
  mysql_stmt custom_variable_insupdate(qp.prepare_insert_or_update(*ms));

  neb::custom_variable cv;
  cv.default_value = "empty";
  cv.modified = false;
  cv.var_type = 1;
  cv.update_time = 0;

  std::cout << "SEND CUSTOM VARIABLES" << std::endl;
  for (int j = 1; j <= 20; j++) {
    for (int i = 1; i <= 30; i++) {
      cv.host_id = j;
      std::ostringstream oss;
      oss << "cv_" << i << "_" << j;
      cv.name = oss.str();
      oss.str("");
      oss << "v" << i << "_" << j;
      cv.value = oss.str();

      custom_variable_insupdate << cv;
      ms->run_statement(custom_variable_insupdate, "ERROR CV STATEMENT", true);
    }
  }
  std::cout << "SEND CUSTOM VARIABLES => DONE" << std::endl;
  ms->commit();
  std::cout << "COMMIT => DONE" << std::endl;
  std::string query(
      "SELECT count(*) FROM customvariables WHERE service_id = 0");
  std::promise<mysql_result> promise;
  ms->run_query_and_get_result(query, &promise);
  mysql_result res(promise.get_future().get());

  ASSERT_TRUE(ms->fetch_row(res));
  std::cout << "***** count = " << res.value_as_i32(0) << std::endl;
  ASSERT_TRUE(res.value_as_i32(0) == 600);
}

TEST_F(DatabaseStorageTest, SelectStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  std::string query("SELECT value,status FROM data_bin WHERE ctime >= ?");
  mysql_stmt select_stmt(ms->prepare_query(query));
  select_stmt.bind_value_as_u64(0, time(nullptr) - 20);
  std::promise<mysql_result> promise;
  ms->run_statement_and_get_result(select_stmt, &promise);
  mysql_result res(promise.get_future().get());

  while (ms->fetch_row(res)) {
    ASSERT_TRUE(res.value_as_f64(0) == 2.5);
    ASSERT_TRUE(res.value_as_i32(1) == 0);
  }
}

TEST_F(DatabaseStorageTest, DowntimeStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));

  std::string query(
      "INSERT INTO downtimes"
      " (actual_end_time, "
      "actual_start_time, "
      "author, type, deletion_time, duration, end_time, entry_time, "
      "fixed, host_id, instance_id, internal_id, service_id, "
      "start_time, triggered_by, cancelled, started, comment_data) "
      "VALUES(:actual_end_time,:actual_start_time,:author,:type,:deletion_"
      "time,:duration,:end_time,:entry_time,:fixed,:host_id,:instance_id,:"
      "internal_id,:service_id,:start_time,:triggered_by,:cancelled,:"
      "started,:comment_data) ON DUPLICATE KEY UPDATE "
      "actual_end_time=GREATEST(COALESCE(actual_end_time, -1), "
      ":actual_end_time),"
      "actual_start_time=COALESCE(actual_start_time, :actual_start_time),"
      "author=:author, cancelled=:cancelled, comment_data=:comment_data,"
      "deletion_time=:deletion_time, duration=:duration, end_time=:end_time,"
      "fixed=:fixed, host_id=:host_id, service_id=:service_id,"
      "start_time=:start_time, started=:started,"
      "triggered_by=:triggered_by, type=:type");
  mysql_stmt downtime_insupdate(mysql_stmt(query, true));
  ms->prepare_statement(downtime_insupdate);

  time_t now(time(nullptr));

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
  std::promise<int> promise;
  ms->run_statement_and_get_int(downtime_insupdate, &promise,
                                mysql_task::int_type::AFFECTED_ROWS);

  std::cout << "downtime_insupdate: " << downtime_insupdate.get_query()
            << std::endl;

  ASSERT_TRUE(promise.get_future().get() == 1);
  ms->commit();
}

TEST_F(DatabaseStorageTest, HostGroupMemberStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));

  ms->run_query("DELETE FROM hostgroups");
  ms->run_query("DELETE FROM hosts_hostgroups");
  ms->commit();

  query_preparator::event_unique unique;
  unique.insert("hostgroup_id");
  unique.insert("host_id");
  query_preparator qp(neb::host_group_member::static_type(), unique);
  mysql_stmt host_group_member_insert(qp.prepare_insert(*ms));

  query_preparator::event_unique unique1;
  unique1.insert("hostgroup_id");
  query_preparator qp1(neb::host_group::static_type(), unique1);
  mysql_stmt host_group_insupdate(qp1.prepare_insert_or_update(*ms));

  neb::host_group_member hgm;
  hgm.enabled = true;
  hgm.group_id = 8;
  hgm.group_name = "Test host group";
  hgm.host_id = 24;
  hgm.poller_id = 1;

  host_group_member_insert << hgm;
  std::cout << host_group_member_insert.get_query() << std::endl;

  std::promise<mysql_result> promise;

  int thread_id(
      ms->run_statement_and_get_result(host_group_member_insert, &promise));
  try {
    promise.get_future().get();
  } catch (std::exception const& e) {
    neb::host_group hg;
    hg.id = 8;
    hg.name = "Test hostgroup";
    hg.enabled = true;
    hg.poller_id = 1;

    host_group_insupdate << hg;

    std::cout << host_group_insupdate.get_query() << std::endl;

    ms->run_statement(host_group_insupdate,
                      "Error: Unable to create host group", true, thread_id);

    host_group_member_insert << hgm;
    ms->run_statement(host_group_member_insert, "Error: host group not defined",
                      true, thread_id);
  }
  ms->commit();
}

TEST_F(DatabaseStorageTest, HostParentStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));

  query_preparator qp(neb::host_parent::static_type());
  mysql_stmt host_parent_insert(qp.prepare_insert(*ms, true));
  query_preparator::event_unique unique;
  unique.insert("child_id");
  unique.insert("parent_id");
  query_preparator qp_del(neb::host_parent::static_type(), unique);
  mysql_stmt host_parent_delete = qp_del.prepare_delete(*ms);

  neb::host_parent hp;
  hp.enabled = true;
  hp.host_id = 24;
  hp.parent_id = 1;

  // Insert.
  host_parent_insert << hp;
  std::promise<int> promise;
  int thread_id(ms->run_statement_and_get_int(
      host_parent_insert, &promise, mysql_task::int_type::AFFECTED_ROWS));

  ASSERT_TRUE(promise.get_future().get() == 1);

  promise = std::promise<int>();
  // Second insert attempted just for the check
  ms->run_statement_and_get_int(host_parent_insert, &promise,
                                mysql_task::int_type::AFFECTED_ROWS, thread_id);

  ASSERT_TRUE(promise.get_future().get() == 0);

  // Disable parenting.
  hp.enabled = false;

  host_parent_delete << hp;
  promise = std::promise<int>();
  ms->run_statement_and_get_int(host_parent_delete, &promise,
                                mysql_task::int_type::AFFECTED_ROWS, thread_id);

  ASSERT_TRUE(promise.get_future().get() == 1);
  ms->commit();
}

TEST_F(DatabaseStorageTest, ServiceGroupMemberStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));

  ms->run_query("DELETE FROM servicegroups");
  ms->run_query("DELETE FROM services_servicegroups");
  ms->commit();

  query_preparator::event_unique unique;
  unique.insert("servicegroup_id");
  unique.insert("host_id");
  unique.insert("service_id");
  query_preparator qp(neb::service_group_member::static_type(), unique);
  mysql_stmt service_group_member_insert(qp.prepare_insert(*ms));

  query_preparator::event_unique unique1;
  unique1.insert("servicegroup_id");
  query_preparator qp1(neb::service_group::static_type(), unique1);
  mysql_stmt service_group_insupdate(qp1.prepare_insert_or_update(*ms));

  neb::service_group_member sgm;
  sgm.enabled = false;
  sgm.group_id = 8;
  sgm.group_name = "Test service group";
  sgm.host_id = 24;
  sgm.service_id = 78;
  sgm.poller_id = 1;

  service_group_member_insert << sgm;

  std::promise<mysql_result> promise;

  int thread_id(
      ms->run_statement_and_get_result(service_group_member_insert, &promise));
  ASSERT_THROW(promise.get_future().get(), std::exception);
  neb::service_group sg;
  sg.id = 8;
  sg.name = "Test servicegroup";
  sg.enabled = true;
  sg.poller_id = 1;

  service_group_insupdate << sg;

  ms->run_statement(service_group_insupdate,
                    "Error: Unable to create service group", true, thread_id);

  promise = std::promise<mysql_result>();
  service_group_member_insert << sgm;
  ms->run_statement_and_get_result(service_group_member_insert, &promise,
                                   thread_id);
  ASSERT_NO_THROW(promise.get_future().get());
  ms->commit();
}

//// Given a mysql object
//// When a prepare statement is done
//// Then we can bind several rows of values to it and execute the statement.
//// Then a commit makes data available in the database.
// TEST_F(DatabaseStorageTest, PrepareBulkQuery) {
//  database_config db_cfg(
//    "MySQL",
//    "127.0.0.1",
//    3306,
//    "centreon",
//    "centreon",
//    "centreon_storage",
//    5,
//    true,
//    5);
//  time_t now(time(NULL));
//  std::ostringstream oss;
//  oss << "INSERT INTO " << "metrics"
//      << "  (index_id, metric_name, unit_name, warn, warn_low,"
//         "   warn_threshold_mode, crit, crit_low, "
//         "   crit_threshold_mode, min, max, current_value,"
//         "   data_source_type)"
//         " VALUES (?, ?, ?, ?, "
//         "         ?, ?, ?, "
//         "         ?, ?, ?, ?, "
//         "         ?, ?)";
//
//  std::unique_ptr<mysql> ms(new mysql(db_cfg));
//  std::ostringstream nss;
//  nss << "metric_name - " << time(NULL);
//  mysql_stmt stmt(ms->prepare_query(oss.str()));
//
//  // Rows are just put on the same row one after the other. The important
//  thing
//  // is to know the length of a row in bytes.
//  stmt.set_array_size(2);
//  for (int i(0); i < 2; ++i) {
//    stmt.bind_value_as_i32(0 + i * 13, 19);
//    stmt.bind_value_as_str(1 + i * 13, nss.str());
//    stmt.bind_value_as_str(2 + i * 13, "test/s");
//    stmt.bind_value_as_f32(3 + i * 13, NAN);
//    stmt.bind_value_as_f32(4 + i * 13, INFINITY);
//    stmt.bind_value_as_tiny(5 + i * 13, true);
//    stmt.bind_value_as_f32(6 + i * 13, 10.0);
//    stmt.bind_value_as_f32(7 + i * 13, 20.0);
//    stmt.bind_value_as_tiny(8 + i * 13, false);
//    stmt.bind_value_as_f32(9 + i * 13, 0.0);
//    stmt.bind_value_as_f32(10 + i * 13, 50.0);
//    stmt.bind_value_as_f32(11 + i * 13, 1 + 2 * i);
//    stmt.bind_value_as_str(12 + i * 13, "2");
//  }
//  // We force the thread 0
//  ms->run_statement(stmt, NULL, "", false, 0);
//  oss.str("");
//  oss << "SELECT metric_name FROM metrics WHERE metric_name='" << nss.str() <<
//  "'"; std::promise<mysql_result> promise; ms->run_query(oss.str(), &promise);
//  mysql_result res(promise.get_future().get());
//  ASSERT_FALSE(ms->fetch_row(res));
//  ASSERT_NO_THROW(ms->commit());
//  promise = std::promise<mysql_result>();
//  ms->run_query(oss.str(), &promise);
//  res = promise.get_future().get();
//  ASSERT_TRUE(ms->fetch_row(res));
//}

TEST_F(DatabaseStorageTest, ChooseConnectionByName) {
  modules::loader l;
  database_config db_cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                         "centreon_storage", 5, true, 5);
  std::unique_ptr<mysql> ms(new mysql(db_cfg));
  int thread_foo(ms->choose_connection_by_name("foo"));
  int thread_bar(ms->choose_connection_by_name("bar"));
  int thread_boo(ms->choose_connection_by_name("boo"));
  int thread_foo1(ms->choose_connection_by_name("foo"));
  int thread_bar1(ms->choose_connection_by_name("bar"));
  int thread_boo1(ms->choose_connection_by_name("boo"));
  ASSERT_EQ(thread_foo, 0);
  ASSERT_EQ(thread_bar, 1);
  ASSERT_EQ(thread_boo, 2);
  ASSERT_EQ(thread_foo, thread_foo1);
  ASSERT_EQ(thread_bar, thread_bar1);
  ASSERT_EQ(thread_boo, thread_boo1);
}
