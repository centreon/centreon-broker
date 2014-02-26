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



#include "com/centreon/broker/bam/configuration/db.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  constructor
 */
db::db( std::string const& type,
	unsigned short port,
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
  _name(name)
{}


/**
 *  destructor
 *
 */
db::~db(){
}


/**
 *  assignment operator
 *
 */
db& db::operator( db const& other(){
    if( &other != this){
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
   */
  void db::db( db const& other())
  _type (other._type),
  _host (ther._host),
  _port (other._port),
  _user (other._user),
  _password (other._password),
  _name ( other._name)
  {
  }

  /**
   *  get_type
   */  
  std::string const & db::get_type(){ 
    return _type; 
  }

  /**
   * get_host
   */  
  std::string const & db::get_host(){ 
    return _host; 
  }

  /**
   * get_port
   */  
  unsigned short db::get_port(){ 
    return _port; 
  }

  /**
   * get_user
   */  
  std::string const& db::get_user(){ 
    return _user; 
  }

  /**
   * get_password
   */  
  std::string const& db::get_password(){ 
    return _password; 
  }

  /**
   * get_name
   */  
  std::string const& db::get_name(){ 
    return _name; 
  }
  
  /**
   * set_type
   *
   * @param db type
   */  
  void db::set_type(std::string const& t){ 
    _type = t; 
  }

  /**
   * set_host
   */  
  void db::set_host(std::string const& h){ 
    _host = h; 
  }

  /**
   * set_port
   */  
  void db::set_port(unsigned short  p){ 
    _port = p; 
  }

  /**
   * set_user
   */  
  void db::set_user(std::string const&  u){ 
    _user = u; 
  }

  /**
   * set_password
   */  
  void db::set_password(std::string const& p ){ 
    _password = p; 
  }

  /**
   * set_name
   */  
  void db::set_name(std::string const&  n){ 
    _name = n; 
  }








