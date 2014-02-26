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


#include "com/centreon/broker/bam/configuration/ba.hh"
using namespace com::centreon::broker::configuration;

  /**
   *
   *
   *
   */
  ba::ba( unsigned int id,
	  const std::string& name,
	  double level,
	  double warning_level,
	  double critical_level):
  _id(id),
  _name(name),
  _level(level),
  _warning_level(warning_level),
  _critical_level(critical_level)
  {
    
  }  

  /**
   *
   *
   */
  unsigned int ba::get_id()const{ 
    return _id; 
  }

  /**
   *
   *
   */
  std::string const& ba::get_name()const{ 
    return _name;
  }

  /**
   *
   *
   */
  double ba::get_level()const{ 
    return _level;
  }

  /**
   *
   *
   */
  double ba::get_warning_level()const{ 
    return _warning_level;
  }

  /**
   *
   *
   */
  double ba::get_critical_level()const{ 
    return _critical_level;
  }

  /**
   *
   *
   */
  void ba::set_id( unsigned int id){ 
    _id=id;
  }

  /**
   *
   *
   */
  void ba::set_name ( const std::string& name){ 
    _name=name;
  }

  /**
   *
   *
   */
  void ba::set_level ( double level){ 
    _level=level; 
  }

  /**
   *
   *
   */
  void ba::set_warning_level ( double warning_level){ 
    _warning_level=warning_level; 
  }  

  /**
   *
   *
   */
  void ba::set_critical_level ( double critical_level){ 
    _critical_level=critical_level; 
  }



