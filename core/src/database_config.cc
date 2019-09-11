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

#include "com/centreon/broker/database_config.hh"
#include <map>
#include <string>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/config.hh"

using namespace com::centreon::broker;

/**
 *  Default constructor.
 */
database_config::database_config()
    : _queries_per_transaction(1),
      _check_replication(true),
      _connections_count(1) {}

/**
 *  Constructor.
 *
 *  @param[in] type                     DB type ("mysql", "oracle",
 *                                      ...).
 *  @param[in] host                     The host machine.
 *  @param[in] port                     Connection port.
 *  @param[in] user                     The user login.
 *  @param[in] password                 The password.
 *  @param[in] name                     Database name.
 *  @param[in] queries_per_transaction  Number of queries allowed within
 *                                      a transaction before a commit
 *                                      occurs.
 *  @param[in] check_replication        Whether or not the replication
 *                                      status of the database should be
 *                                      checked.
 */
database_config::database_config(std::string const& type,
                                 std::string const& host,
                                 unsigned short port,
                                 std::string const& user,
                                 std::string const& password,
                                 std::string const& name,
                                 int queries_per_transaction,
                                 bool check_replication,
                                 int connections_count)
    : _type(type),
      _host(host),
      _port(port),
      _user(user),
      _password(password),
      _name(name),
      _queries_per_transaction(queries_per_transaction),
      _check_replication(check_replication),
      _connections_count(connections_count) {}

/**
 *  Build a database configuration from a configuration set.
 *
 *  @param[in] cfg  Endpoint configuration.
 */
database_config::database_config(config::endpoint const& cfg) {
  std::map<std::string, std::string>::const_iterator it, end;
  end = cfg.params.end();

  // db_type
  it = cfg.params.find("db_type");
  if (it != end)
    _type = it->second;
  else
    throw(exceptions::config()
          << "no 'db_type' defined for endpoint '" << cfg.name << "'");

  // db_host
  it = cfg.params.find("db_host");
  if (it != end)
    _host = it->second;
  else
    _host = "localhost";

  // db_port
  it = cfg.params.find("db_port");
  if (it != end)
    _port = std::stol(it->second);
  else
    _port = 0;

  // db_user
  it = cfg.params.find("db_user");
  if (it != end)
    _user = it->second;

  // db_password
  it = cfg.params.find("db_password");
  if (it != end)
    _password = it->second;

  // db_name
  it = cfg.params.find("db_name");
  if (it != end)
    _name = it->second;
  else
    throw exceptions::config()
        << "no 'db_name' defined for endpoint '" << cfg.name << "'";

  // queries_per_transaction
  it = cfg.params.find("queries_per_transaction");
  if (it != end)
    _queries_per_transaction = std::stoul(it->second);
  else
    _queries_per_transaction = 20000;

  // check_replication
  it = cfg.params.find("check_replication");
  if (it != end)
    _check_replication = config::parser::parse_boolean(it->second);
  else
    _check_replication = true;

  // connections_count
  it = cfg.params.find("connections_count");
  if (it != end)
    _connections_count = std::stoul(it->second);
  else
    _connections_count = 1;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
database_config::database_config(database_config const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
database_config::~database_config() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
database_config& database_config::operator=(database_config const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}
/**
 *  Comparaison operator.
 *
 *  @param[in] other  Object to compared.
 *
 *  @return true if equal.
 */
bool database_config::operator==(database_config const& other) {
  if (this != &other)
    return _type == other._type && _host == other._host &&
           _port == other._port && _user == other._user &&
           _password == other._password && _name == other._name &&
           _queries_per_transaction == other._queries_per_transaction &&
           _check_replication == other._check_replication &&
           _connections_count == other._connections_count;

  return true;
}

/**
 *  Get DB type.
 *
 *  @return The DB type.
 */
std::string const& database_config::get_type() const {
  return (_type);
}

/**
 *  Get the DB host.
 *
 *  @return The DB host
 */
std::string const& database_config::get_host() const {
  return (_host);
}

/**
 *  Get the connection port.
 *
 *  @return The connection port.
 */
unsigned short database_config::get_port() const {
  return (_port);
}

/**
 *  Get user.
 *
 *  @return The user.
 */
std::string const& database_config::get_user() const {
  return (_user);
}

/**
 *  Get password.
 *
 *  @return The password.
 */
std::string const& database_config::get_password() const {
  return (_password);
}

/**
 *  Get DB name.
 *
 *  @return The database name.
 */
std::string const& database_config::get_name() const {
  return (_name);
}

/**
 *  Get the number of queries per transaction.
 *
 *  @return Number of queries per transaction.
 */
int database_config::get_queries_per_transaction() const {
  return (_queries_per_transaction);
}

/**
 *  Check whether or not database replication should be checked.
 *
 *  @return Database replication check flag.
 */
bool database_config::get_check_replication() const {
  return (_check_replication);
}

/**
 *  Get the number of connections to open to the database server.
 *
 *  @return Number of connections.
 */
int database_config::get_connections_count() const {
  return (_connections_count);
}

/**
 *  Set type.
 *
 *  @param[in] type  The database type.
 */
void database_config::set_type(std::string const& type) {
  _type = type;
}

/**
 *  Set host.
 *
 *  @param[in] host  The host.
 */
void database_config::set_host(std::string const& host) {
  _host = host;
}

/**
 *  Set port.
 *
 *  @param[in] port  Set the port number of the database server.
 */
void database_config::set_port(unsigned short port) {
  _port = port;
}

/**
 *  Set user.
 *
 *  @param[in] user  The user name.
 */
void database_config::set_user(std::string const& user) {
  _user = user;
}

/**
 *  Set password.
 *
 *  @param[in] password  The password.
 */
void database_config::set_password(std::string const& password) {
  _password = password;
}

/**
 *  Set the database name.
 *
 *  @param[in] name  The database name.
 */
void database_config::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Set the number of queries per transaction.
 *
 *  @param[in] qpt  Number of queries per transaction.
 */
void database_config::set_queries_per_transaction(int qpt) {
  _queries_per_transaction = qpt;
}

/**
 *  Set the number of connections.
 *
 *  @param[in] qpt  Number of connections.
 */
void database_config::set_connections_count(int count) {
  _connections_count = count;
}

/**
 *  Set whether or not database replication should be checked.
 *
 *  @param[in] check_replication  Replication check flag.
 */
void database_config::set_check_replication(bool check_replication) {
  _check_replication = check_replication;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void database_config::_internal_copy(database_config const& other) {
  _type = other._type;
  _host = other._host;
  _port = other._port;
  _user = other._user;
  _password = other._password;
  _name = other._name;
  _queries_per_transaction = other._queries_per_transaction;
  _check_replication = other._check_replication;
  _connections_count = other._connections_count;
}
