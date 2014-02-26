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


#include "com/centreon/broker/bam/configuration/ba.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] id             BA ID.
 *  @param[in] name           BA name.
 *  @param[in] level          BA level.
 *  @param[in] warning_level  BA warning_level.
 *  @param[in] critical_level BA critical_level.
 *  
 */
ba::ba(
       unsigned int id,
       std::string const& name,
       double level,
       double warning_level,
       double critical_level):
  _id(id),
  _name(name),
  _level(level),
  _warning_level(warning_level),
  _critical_level(critical_level)
{}  


/**
 *  Copie Constructor.
 *  
 */
ba::ba(ba const& other)
:
  _id( other._id ),
  _name( other._name ),
  _level( other._level ),
  _warning_level( other._warning_level ),
  _critical_level( other._critical_level ) 
{
}

/**
 *  Assignment Operator.
 *  
 */
ba& ba::operator=(ba const& other){
  if ( this != &other){
    _id = other._id;
    _name =  other._name;
    _level = other._level;
    _warning_level = other._warning_level;
    _critical_level = other._critical_level;
  }
  return *this;
}

/**
 *  destructor.
 *  
 */
void bool_expression::~bool_expression(){
}



/**
 * get_id
 * 
 */
 unsigned int ba::get_id() const {
  return (_id);
}

/**
 * get_name
 * 
 */
std::string const& ba::get_name() const {
  return (_name);
}

/**
 * get_level
 *
 */
double ba::get_level()const{ 
  return _level;
}

/**
 * get_warning_level
 *
 */
double ba::get_warning_level()const{ 
  return _warning_level;
}

/**
 * get_critical_level
 *
 */
double ba::get_critical_level()const{ 
  return _critical_level;
}

/**
 * set_id
 *
 * @param id
 */
void ba::set_id(unsigned int id){ 
  _id=id;
}

/**
 * set_name
 *
 * @param name
 */
void ba::set_name (const std::string& name){ 
  _name=name;
}

/**
 * set_level
 *
 * @param level
 */
void ba::set_level (double level){ 
  _level=level; 
}

/**
 * set_warning_level
 *
 * @param warning_level
 */
void ba::set_warning_level(double warning_level){ 
  _warning_level=warning_level; 
}  

/**
 * set_criticial_level
 *
 * @param critical_level
 */
void ba::set_critical_level(double critical_level){ 
  _critical_level=critical_level; 
}



