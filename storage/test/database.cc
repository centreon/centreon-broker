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

#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/storage/mysql.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

class DatabaseStorageTest : public ::testing::Test {
 public:
  void SetUp() {}
};

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

TEST_F(DatabaseStorageTest, SendDataBin) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "centreon",
    "centreon_storage",
    1,
    true,
    5);
  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  ms->run_query("INSERT INTO data_bin (id_metric, ctime, status, value) VALUES " \
      "(1, 1533568152, '0', 2.5)");
}

static void callback_get_insert_id(MYSQL* conn) {
  int id(mysql_insert_id(conn));

  mysql_query(conn, "SELECT MAX(comment_id) FROM comments");
  MYSQL_RES* result = mysql_store_result(conn);
  int num_fields(mysql_num_fields(result));
  ASSERT_TRUE(num_fields == 1);
  MYSQL_ROW row(mysql_fetch_row(result));
  ASSERT_TRUE(atoi(row[0]) == id);
  mysql_free_result(result);
  std::cout << "ID IS GOOD: " << id << std::endl;
}

TEST_F(DatabaseStorageTest, QueryWithCallback) {
  database_config db_cfg(
    "MySQL",
    "127.0.0.1",
    3306,
    "root",
    "centreon",
    "centreon_storage",
    1,
    true,
    5);
  time_t now(time(NULL));
  std::ostringstream oss;
    oss << "INSERT INTO comments (internal_id, host_id, entry_time, author, data) "
      << "VALUES (1, 1, " << now
      << ", 'test-user', 'comment from InsertAndGetInsertId1')";

  std::auto_ptr<mysql> ms(new mysql(db_cfg));
  ms->run_query_with_callback(oss.str(), callback_get_insert_id);
}
