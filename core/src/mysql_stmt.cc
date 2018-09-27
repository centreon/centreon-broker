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

#include <iostream>
#include <QString>
#include "com/centreon/broker/mysql_bind.hh"
#include "com/centreon/broker/mysql_stmt.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

void static mysql_stmt_deleter(MYSQL_STMT* stmt) {
  mysql_stmt_close(stmt);
}

mysql_stmt::mysql_stmt()
 : _stmt(NULL, mysql_stmt_deleter) {
  std::cout << "mysql_stmt: empty constructor..." << std::endl;
}

mysql_stmt::mysql_stmt(MYSQL* conn,
              std::string const& query,
              mysql_stmt_mapping const& bind_mapping)
 : _bind_mapping(bind_mapping) {
  std::cout << "mysql_stmt: constructor with arguments..." << std::endl;
  MYSQL_STMT* stmt(mysql_stmt_init(conn));
  _stmt = misc::shared_ptr<MYSQL_STMT>(stmt, mysql_stmt_deleter);
  if (!stmt) {
    throw exceptions::msg()
      << "mysql: Could not initialize statement";
  }
  else {
    if (mysql_stmt_prepare(_stmt.data(), query.c_str(), query.size())) {
      throw exceptions::msg()
        << "mysql: Could not prepare statement";
    }
  }
  std::cout << "mysql_stmt: End of constructor with arguments..." << std::endl;
}

mysql_stmt::mysql_stmt(mysql_stmt const& other) {
  std::cout << "mysql_stmt: constructor by copy..." << std::endl;
  _stmt = other._stmt;
  _bind_mapping = other._bind_mapping;
}

mysql_stmt::~mysql_stmt() {
  std::cout << "mysql_stmt: destructor" << std::endl;
}

mysql_stmt& mysql_stmt::operator=(mysql_stmt const& other) {
  std::cout << "mysql_stmt: operator=..." << std::endl;
  if (this != &other) {
    _stmt = other._stmt;
    _bind_mapping = other._bind_mapping;
  }
  return *this;
}

int mysql_stmt::bind(mysql_bind const& bind) {
  return mysql_stmt_bind_param(_stmt.data(), const_cast<MYSQL_BIND*>(bind.get_bind()));
}

char const* mysql_stmt::get_error() {
  return mysql_stmt_error(_stmt.data());
}

int mysql_stmt::execute() {
  return mysql_stmt_execute(_stmt.data());
}

mysql_stmt_mapping mysql_stmt::get_mapping() const {
  return _bind_mapping;
}
