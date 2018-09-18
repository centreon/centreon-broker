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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/mysql.hh"

using namespace com::centreon::broker;

class DatabaseStorageTest : public ::testing::Test {
 public:
  void SetUp() {}
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
    "centreon",
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
    "centreon",
    "centreon_storage",
    5,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  std::ostringstream oss;
  int now(time(NULL));
  oss << "INSERT INTO data_bin (id_metric, ctime, status, value) VALUES "
      << "(1, " << now << ", '0', 2.5)";
  ms->run_query(oss.str());
  oss.str("");
  oss << "SELECT id_metric, status FROM data_bin WHERE ctime=" << now;
  int thread_id(ms->run_query_sync(oss.str()));
  mysql_result res(ms->get_result(thread_id));
  ASSERT_FALSE(res.next());
  ASSERT_NO_THROW(ms->commit());
  thread_id = ms->run_query_sync(oss.str());
  res = ms->get_result(thread_id);
  ASSERT_TRUE(res.next());
}

static int callback_get_insert_id(MYSQL* conn, void* data) {
  int id(mysql_insert_id(conn));

  mysql_query(conn, "SELECT MAX(comment_id) FROM comments");
  MYSQL_RES* result = mysql_store_result(conn);
  int num_fields(mysql_num_fields(result));
  EXPECT_EQ(num_fields, 1);
  MYSQL_ROW row(mysql_fetch_row(result));
  EXPECT_TRUE(atoi(row[0]) == id);
  mysql_free_result(result);
  return 0;
}

// Given a mysql object
// When a query is done with a callback
// Then a thread is chosen to make the query and then it calls the callback function.
TEST_F(DatabaseStorageTest, QueryWithCallback) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "centreon",
    "centreon_storage",
    5,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream oss;
  oss << "INSERT INTO comments (internal_id, host_id, entry_time, author, data) "
    << "VALUES (1, 1, " << now
    << ", 'test-user', 'comment from InsertAndGetInsertId1')";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  ms->run_query(oss.str(), "", false, callback_get_insert_id, NULL);
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
    "centreon",
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
  int stmt_id(ms->prepare_query(oss.str()));
  mysql_bind bind(13);
  bind.set_int(0, 19);
  bind.set_string(1, nss.str());
  bind.set_string(2, "test/s");
  bind.set_float(3, NAN);
  bind.set_float(4, INFINITY);
  bind.set_tiny(5, true);
  bind.set_float(6, 10.0);
  bind.set_float(7, 20.0);
  bind.set_tiny(8, false);
  bind.set_float(9, 0.0);
  bind.set_float(10, 50.0);
  bind.set_float(11, 18.0);
  bind.set_string(12, "2");
  // We force the thread 0
  ms->run_statement(stmt_id, bind, "", false, 0, 0, 0);
  oss.str("");
  oss << "SELECT metric_name FROM metrics WHERE metric_name='" << nss.str() << "'";
  int thread_id(ms->run_query_sync(oss.str()));
  mysql_result res(ms->get_result(thread_id));
  ASSERT_FALSE(res.next());
  ASSERT_NO_THROW(ms->commit());
  thread_id = ms->run_query_sync(oss.str());
  res = ms->get_result(thread_id);
  ASSERT_TRUE(res.next());
}

TEST_F(DatabaseStorageTest, QuerySync) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "centreon",
    "centreon_storage",
    5,
    true,
    5);
  std::ostringstream oss;
  oss << "SELECT comment_id, persistent, data FROM comments LIMIT 10";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  int id(ms->run_query_sync(oss.str()));
  mysql_result res(ms->get_result(id));
  int count(0);
  while (res.next()) {
    int v(res.value_as_i32(0));
    int t(res.value_as_bool(1));
    std::string s(res.value_as_str(2));
    ASSERT_GT(v, 0);
    ASSERT_FALSE(t);
    ASSERT_FALSE(s.empty());
    std::cout << "value " << v << std::endl;
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
    "centreon",
    "centreon_storage",
    5,
    true,
    5);

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  ASSERT_THROW(
    ms->run_query_sync("SELECT foo FROM bar LIMIT 1"),
    exceptions::msg);
}

TEST_F(DatabaseStorageTest, QueryWithError) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "centreon",
    "centreon_storage",
    5,
    true,
    5);

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  // The following insert fails
  ms->run_query("INSERT INTO FOO (toto) VALUES (0)", "", true, 0, 0, 1);
  ms->commit();

  // The following is the same one, executed by the same thread but since the
  // previous error, an exception should arrive.
  ASSERT_THROW(ms->run_query("INSERT INTO FOO (toto) VALUES (0)", "", true, 0, 0, 1), std::exception);
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
    "centreon",
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
  int thread_id(ms->run_query_sync(oss.str()));
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
  thread_id = ms->run_query_sync(oss.str());
  mysql_result res(ms->get_result(thread_id));
  ASSERT_TRUE(res.next());
  ASSERT_TRUE(res.value_as_i32(0) == id);
}

TEST_F(DatabaseStorageTest, PrepareQuerySync) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "centreon",
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
  int stmt_id(ms->prepare_query(oss.str()));
  mysql_bind bind(13);
  bind.set_int(0, 19);
  bind.set_string(1, nss.str());
  bind.set_string(2, "test/s");
  bind.set_float(3, 20.0);
  bind.set_float(4, 40.0);
  bind.set_tiny(5, 1);
  bind.set_float(6, 10.0);
  bind.set_float(7, 20.0);
  bind.set_tiny(8, 1);
  bind.set_float(9, 0.0);
  bind.set_float(10, 50.0);
  bind.set_float(11, 18.0);
  bind.set_string(12, "2");
  // We force the thread 0
  int thread_id(ms->run_statement_sync(stmt_id, bind, "", 0));
  int id(ms->get_last_insert_id(thread_id));
  ASSERT_TRUE(id > 0);
  std::cout << "id = " << id << std::endl;
  oss.str("");
  oss << "SELECT metric_id FROM metrics WHERE metric_name='" << nss.str() << "'";
  thread_id = ms->run_query_sync(oss.str());
  mysql_result res(ms->get_result(thread_id));
  ASSERT_FALSE(res.next());
  ASSERT_NO_THROW(ms->commit());
  thread_id = ms->run_query_sync(oss.str());
  res = ms->get_result(thread_id);
  ASSERT_TRUE(res.next());
  std::cout << "id1 = " << res.value_as_i32(0) << std::endl;
  ASSERT_TRUE(res.value_as_i32(0) == id);
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
    "centreon",
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
  int stmt_id(ms->prepare_query(oss.str()));
  for (int i(1); i < 4000; ++i) {
    mysql_bind bind(11);
    bind.set_string(0, "test/s");
    bind.set_float(1, NAN);
    bind.set_float(2, NAN);
    bind.set_tiny(3, 0);
    bind.set_float(4, NAN);
    bind.set_float(5, NAN);
    bind.set_tiny(6, 0);
    bind.set_float(7, 10.0);
    bind.set_float(8, 20.0);
    bind.set_float(9, 18.0);
    bind.set_int(10, i);

    ms->run_statement(stmt_id, bind);
  }
  ms->commit();
}

