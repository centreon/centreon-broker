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
 *  constructor
 *
 *  param[in]  type
 *  param[in]  the port
 *  param[in]  the host machine
 *  param[in]  the user login
 *  param[in]  the password
 *  param[in]  database name
 */
    db::db(std::string const& type,
	   unsigned short     port,
	   std::string const& host,
	   std::string const& user,
	   std::string const& password,
	   std::string const& name
      ):
    _type(type),
    _host(host),
    _port(port),
    _user(user),
    _password(password),
    _name(name){
    }


/**
 *  destructor
 *
 */
  db::~db(){
  }


/**
 *  assignment operator
 *
 *  param[in]  other
 */
  db& db::operator=(db const& other){
    if(&other != this){
      _type = other._type;
      _host = other._host;
      _port = other._port;
      _user = other._user;
      _password = other._password;
      _name = other._name;
    }
    return *this;
  }


/**
 * Copy constructor
 *
 *  param[in]  other
 */
  db::db( db const& other):
    _type (other._type),
    _host (other._host),
    _port (other._port),
    _user (other._user),
    _password (other._password),
    _name ( other._name)
    {
    }

/**
 *  get type
 *
 *  @return The type
 */
  std::string const & db::get_type()const{
    return _type;
  }

/**
 *  get host
 *
 *  @return The host
 */
  std::string const & db::get_host()const{
    return _host;
  }

/**
 *  get port
 *
 *  @return The port
 */
  unsigned short db::get_port()const{
    return _port;
  }

/**
 *  get user
 *
 *  @return The user
 */
  std::string const& db::get_user()const{
    return _user;
  }

/**
 *  get password
 *
 *  @return The password
 */
  std::string const& db::get_password()const{
    return _password;
  }

/**
 *  get name
 *
 *  @return The database name
 */
  std::string const& db::get_name()const{
    return _name;
  }

/**
 *  set type
 *
 * @param The database type
 */
  void db::set_type(std::string const& t){
    _type = t;
  }

/**
 *  set host
 *
 *  @param The host
 */
  void db::set_host(std::string const& h){
    _host = h;
  }

/**
 *  set port
 *
 *  @param Set the port number of the database
 */
  void db::set_port(unsigned short  p){
    _port = p;
  }

/**
 *  set user
 *
 *  @param The user name
 */
  void db::set_user(std::string const&  u){
    _user = u;
  }

/**
 *  set password
 *
 *  @param password The password
 */
  void db::set_password(std::string const& password ){
    _password = password;
  }

/**
 *  set name
 *
 *  @param[in] name The database name
 */
  void db::set_name(std::string const&  name){
    _name = name;
  }





