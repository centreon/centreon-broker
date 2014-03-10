/*
** Copyright 2009-2013 Merethis
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
#include "com/centreon/broker/bam/configuration/db.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  param[in] type     DB type ("mysql", "oracle", ...).
 *  param[in] port     Connection port.
 *  param[in] host     The host machine.
 *  param[in] user     The user login.
 *  param[in] password The password.
 *  param[in] name     Database name.
 */
db::db(
      std::string const& type,
      unsigned short port,
      std::string const& host,
      std::string const& user,
      std::string const& password,
      std::string const& name)
  : _type(type),
    _host(host),
    _port(port),
    _user(user),
    _password(password),
    _name(name) {}

/**
 *  Copy constructor.
 *
 *  param[in]  other Object to copy.
 */
db::db(db const& other)
  : _type(other._type),
    _host(other._host),
    _port(other._port),
    _user(other._user),
    _password(other._password),
    _name( other._name)
{}

/**
 *  Destructor.
 */
db::~db() {}

/**
 *  Assignment operator.
 *
 *  param[in]  other Object to copy.
 *
 *  @return This object.
 */
db& db::operator=(db const& other) {
  if (&other != this) {
    _type = other._type;
    _host = other._host;
    _port = other._port;
    _user = other._user;
    _password = other._password;
    _name = other._name;
  }
  return (*this);
}

/**
 *  Get DB type.
 *
 *  @return The DB type.
 */
std::string const& db::get_type() const {
  return (_type);
}

/**
 *  Get the DB host.
 *
 *  @return The DB host
 */
std::string const& db::get_host() const {
  return (_host);
}

/**
 *  Get the connection port.
 *
 *  @return The connection port.
 */
unsigned short db::get_port() const {
  return (_port);
}

/**
 *  Get user.
 *
 *  @return The user.
 */
std::string const& db::get_user() const {
  return (_user);
}

/**
 *  Get password.
 *
 *  @return The password.
 */
std::string const& db::get_password() const {
  return (_password);
}

/**
 *  Get DB name.
 *
 *  @return The database name.
 */
std::string const& db::get_name() const {
  return (_name);
}

/**
 *  Set type.
 *
 *  @param[in] t The database type.
 */
void db::set_type(std::string const& t) {
  _type = t;
}

/**
 *  Set host.
 *
 *  @param[in] h The host.
 */
void db::set_host(std::string const& h) {
  _host = h;
}

/**
 *  Set port.
 *
 *  @param[in] p Set the port number of the database.
 */
void db::set_port(unsigned short p) {
  _port = p;
}

/**
 *  Set user.
 *
 *  @param[in] u The user name.
 */
void db::set_user(std::string const& u) {
  _user = u;
}

/**
 *  Set password.
 *
 *  @param[in] password The password.
 */
void db::set_password(std::string const& password) {
  _password = password;
}

/**
 *  Set name.
 *
 *  @param[in] name The database name.
 */
void db::set_name(std::string const& name) {
  _name = name;
}
