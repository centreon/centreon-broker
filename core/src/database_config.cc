/*
** Copyright 2014 Merethis
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

#include <map>
#include "com/centreon/broker/database_config.hh"

using namespace com::centreon::broker;

/**
 *  Default constructor.
 */
database_config::database_config()
  : _queries_per_transaction(1), _check_replication(true) {}

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
database_config::database_config(
                   std::string const& type,
                   std::string const& host,
                   unsigned short port,
                   std::string const& user,
                   std::string const& password,
                   std::string const& name,
                   int queries_per_transaction,
                   bool check_replication)
  : _type(type),
    _host(host),
    _port(port),
    _user(user),
    _password(password),
    _name(name),
    _queries_per_transaction(queries_per_transaction),
    _check_replication(check_replication) {}

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
database_config& database_config::operator=(
                                    database_config const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
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
}
