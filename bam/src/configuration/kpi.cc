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

#include "com/centreon/broker/bam/configuration/kpi.hh"

using namespace com::centreon::broker::bam::configuration;

    
  /**
   * constructor
   */  
  kpi::kpi( unsigned int id,
	    short state_type,
	    unsigned int host_id,
	    unsigned int service_id,
	    unsigned int ba,
	    short status,
	    short last_hard_state,
	    bool downtimed,
	    bool acknowledged,
	    bool ignore_downtime,
	    bool ignore_acknowledgement,
	    double warning,
	    double critical,
	    double unknown):
    _id(id),
    _state_type(state_type),
    _host_id(host_id),
    _service_id(service_id),
    _ba_id(ba),
    _status(status),
    _last_hard_state(last_hard_state),
    _downtimed(downtimed),
    _acknowledged(acknowledged),
    _ignore_downtime(ignore_downtime),
    _ignore_acknowledgement(ignore_acknowledgement),
    _impact_warning(warning),
    _impact_critical(critical),
    _impact_unknown(unknown)  
  {}

/**
 * Copy constructor 
 *
 */
kpi::kpi(kpi const& other)
 :
  _id( other._id),
  _state_type( other._state_type),
  _host_id( other._host_id),
  _service_id( other._service_id),
  _ba_id( other._ba_id),
  _status( other._status),
  _last_hard_state( other._last_hard_state),
  _downtimed( other._downtimed),
  _acknowledged( other._acknowledged),
  _ignore_downtime( other._ignore_downtime),
  _ignore_acknowledgement( other._ignore_acknowledgement),
  _impact_warning( other._impact_warning),
  _impact_critical( other._impact_critical),
  _impact_unknown( other._impact_unknown)
{
}

/**
 * assignment operator
 *
 */
kpi& kpi::operator=(kpi const&){

}



/**
 * get_id
 */  
unsigned kpi::get_id()const{  
  return _id;
}

/**
 * get_state_type
 */  
short kpi::get_state_type()const{
  return  _state_type ;
}

/**
 * get_host_id
 */  
unsigned int kpi::get_host_id()const{
  return   _host_id;
}

/**
 * get_service_id
 */  
unsigned int kpi::get_service_id()const{ 
  return _service_id;
}

/**
 * is_service
 */  
bool kpi::is_service() const { 
  return (_service_id != 0 ) ; 
}  

/**
 * is_ba
 */  
bool kpi::is_ba() const { 
  return (_ba_id != 0); 
}

/**
 * get_bad_id
 */  
unsigned int kpi::get_ba_id()const{
  return  _ba_id ;
}

/**
 * get_status
 */  
short kpi::get_status()const{
  return  _status ;
}

/**
 * get_last_hard_state
 */  
short kpi::get_last_hard_state()const{
  return  _last_hard_state;
}

/**
 * is_downtimed
 */  
bool kpi::is_downtimed() const{  
  return _downtimed;
}

/**
 * is_acknowledged
 */  
bool kpi::is_acknowledged() const{  
  return _acknowledged;
}

/**
 * ignore_downtime
 */  
bool kpi::ignore_downtime() const{  
  return _ignore_downtime;
}

/**
 * ignore_acknowledgement
 */  
bool kpi::ignore_acknowledgement() const{  
  return _ignore_acknowledgement;
}
  
/**
 * get_impact_warning
 */  
double kpi::get_impact_warning() const{  
  return _impact_warning;
}

/**
 * get_impact_critical
 */  
double kpi::get_impact_critical() const{  
  return _impact_critical;
}

/**
 * get_impact_unknown
 */  
double kpi::get_impact_unknown() const{  
  return _impact_unknown;
}

/**
 * set_state_type
 */  
void kpi::set_state_type(short s){  
  _state_type = s ;
}
    
/**
 * set_host_id
 */  
void kpi::set_host_id(unsigned int i){  
  _host_id = i;
}

/**
 * set_service_id
 */  
void kpi::set_service_id(unsigned int i ){  
  _service_id = i;
}

/**
 * set_ba_id
 */  
void kpi::set_ba_id(unsigned int i){  
  _ba_id = i;
}

/**
 * set_status
 */  
void kpi::set_status(short s){  
  _status = s;
}

/**
 * set_last_hard_state
 */  
void kpi::set_last_hard_state(short s){  
  _last_hard_state = s;
}

/**
 * set_downtimed
 */  
void kpi::set_downtimed(bool b){  
  _downtimed = b ;
}

/**
 * set_acknowledged
 */  
void kpi::set_acknowledged(bool b){  
  _acknowledged = b;
}

/**
 * ignore_downtime
 */  
void kpi::ignore_downtime(bool b){  
  _ignore_downtime = b ;
}

/**
 * ignore_acknowledgement
 */  
void kpi::ignore_acknowledgement(bool b){  
  _ignore_acknowledgement = b ;
}

/**
 * set_impact_warning
 */  
void kpi::set_impact_warning(double d){  
  _impact_warning = d ;
}

/**
 * set_impact_critical
 */  
void kpi::set_impact_critical(double d){  
  _impact_critical = d ;
}

/**
 * set_impact_unknown
 */  
void kpi::set_impact_unknown(double d){  
  _impact_unknown = d;
}

  

