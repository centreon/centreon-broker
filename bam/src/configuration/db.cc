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

using namespace com::centreon::broker::configuration;




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
   *
   */  
  std::string const & db::get_type(){ 
    return _type; 
  }

  /**
   *
   */  
  std::string const & db::get_host(){ 
    return _host; 
  }

  /**
   *
   */  
  unsigned short db::get_port(){ 
    return _port; 
  }

  /**
   *
   */  
  std::string const & db::get_user(){ 
    return _user; 
  }

  /**
   *
   */  
  std::string const & db::get_password(){ 
    return _password; 
  }

  /**
   *
   */  
  std::string const & db::get_name(){ 
    return _name; 
  }
  
  /**
   *
   */  
  void db::set_type(std::string const&  t){ 
    _type = t; 
  }

  /**
   *
   */  
  void db::set_host(std::string const& h) { 
    _host = h; 
  }

  /**
   *
   */  
  void db::set_port(unsigned short  p){ 
    _port = p; 
  }

  /**
   *
   */  
  void db::set_user(std::string const&  u){ 
    _user = u; 
  }

  /**
   *
   */  
  void db::set_password(std::string const& p ){ 
    _password = p; 
  }

  /**
   *
   */  
  void db::set_name(std::string const&  n){ 
    _name = n; 
  }








