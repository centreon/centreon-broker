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
kpi::kpi(unsigned int id,
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
 *  Copy constructor 
 *
 *  @param[in] other the copy
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
 *  assignment operator
 *
 *  @param[in] other the copy
 */
kpi& kpi::operator=(kpi const& other){
  if( &other != this){
    _id=other._id;
    _state_type = other._state_type;
    _host_id = other._host_id;
    _service_id = other._service_id;
    _ba_id = other._ba_id;
    _status = other._status;
    _last_hard_state = other._last_hard_state;
    _downtimed = other._downtimed;
    _acknowledged = other._acknowledged;
    _ignore_downtime = other._ignore_downtime;
    _ignore_acknowledgement = other._ignore_acknowledgement;
    _impact_warning = other._impact_warning;
    _impact_critical = other._impact_critical;
    _impact_unknown = other._impact_unknown ;
  }
  return *this;
}


/**
 *  destructor
 *
 */
kpi::~kpi(){
}


/**
 *  get the id
 *
 *  @return  the id value
 */  
unsigned kpi::get_id()const{  
  return _id;
}

/**
 *  get the state type
 *
 *  @return  The state type
 */  
short kpi::get_state_type()const{
  return  _state_type ;
}

/**
 *  get the host id
 *
 *  @return  The host id
 */  
unsigned int kpi::get_host_id()const{
  return   _host_id;
}

/**
 *   get the service id
 *
 *
 *  @return  The host id
 */  
unsigned int kpi::get_service_id()const{ 
  return _service_id;
}

/**
 *  is this kpi a service
 * 
 *  @return Whether this is a service or not
 */  
bool kpi::is_service() const { 
  return (_service_id != 0 ) ; 
}  

/**
 * is this kpi a business activity
 * 
 *  @return Whether this is a business activity abstraction or not
 */  
bool kpi::is_ba() const { 
  return (_ba_id != 0); 
}

/**
 *  get ba id
 *
 *  @return The id of the business activity 
 */  
unsigned int kpi::get_ba_id()const{
  return  _ba_id ;
}

/**
 * get the status
 *
 *  @return The status
 */  
short kpi::get_status()const{
  return  _status ;
}

/**
 *  get the last hard state for this kpi
 *
 *  @return The last business activity confirmed state ( after N verifications)
 */  
short kpi::get_last_hard_state()const{
  return  _last_hard_state;
}

/**
 *  Is this kpi set to downtime mode
 *
 *  @return Has this business interest been downtimed?
 */  
bool kpi::is_downtimed() const{  
  return _downtimed;
}

/**
 *  Is this kpi acknowledged
 *
 *  @return Whether it has been acknowledged
 */  
bool kpi::is_acknowledged() const{  
  return _acknowledged;
}

/**
 * is this kpi to ignore downtime
 *
 *  @return Whether or not the downtime is relevant
 */  
bool kpi::ignore_downtime() const{  
  return _ignore_downtime;
}

/**
 * is this kpi to ignore acknowledgements
 *
 *  @return Whether or not the acknowledgements are applicable
 */  
bool kpi::ignore_acknowledgement() const{  
  return _ignore_acknowledgement;
}
  
/**
 * get impact warning
 *
 *  @return The get business impact of a warning at this level
 */  
double kpi::get_impact_warning() const{  
  return _impact_warning;
}

/**
 * get impact critical
 *
 *  @return The get business impact of a criticality at this level
 */  
double kpi::get_impact_critical() const{  
  return _impact_critical;
}

/**
 * get impact unknown
 *
 *  @return The get business impact of an unknown status at this level
 */  
double kpi::get_impact_unknown() const{  
  return _impact_unknown;
}

/**
 *  set state type
 *
 *  @param[in] Set the state type
 */  
void kpi::set_state_type(short s){  
  _state_type = s ;
}
    
/**
 * set host id
 *
 *  @param[in] Set the host id
 */  
void kpi::set_host_id(unsigned int i){  
  _host_id = i;
}

/**
 * set service id
 *
 *  @param[in] The service id
 */  
void kpi::set_service_id(unsigned int i ){  
  _service_id = i;
}

/**
 * Set the business activity id
 *
 *  @param[in] set the business activity id 
 */  
void kpi::set_ba_id(unsigned int i){  
  _ba_id = i;
}

/**
 * set status
 *
 *  @param[in] Status value
 */  
void kpi::set_status(short s){  
  _status = s;
}

/**
 * set last_hard state
 *
 *  @param[in] Set the last confirmed state for the kpi
 */  
void kpi::set_last_hard_state(short s){  
  _last_hard_state = s;
}

/**
 * set downtimed
 *
 *  @param[in] Set whether the kpi is downtimed or not
 */  
void kpi::set_downtimed(bool b){  
  _downtimed = b ;
}

/**
 *  set acknowledged
 *
 *  @param[in] Set the acknowledged state
 */  
void kpi::set_acknowledged(bool b){  
  _acknowledged = b;
}

/**
 * ignore downtime
 *
 *  @param[in]  Set whether or not the downtime is to be ignored
 */  
void kpi::ignore_downtime(bool b){  
  _ignore_downtime = b ;
}

/**
 * ignore acknowledgement
 *
 *  @param[in]  Set whether or not the acknowledgement is to be ignored
 */  
void kpi::ignore_acknowledgement(bool b){  
  _ignore_acknowledgement = b ;
}

/**
 * set impact warning
 *
 *  @param[in]  Set the business impact of a warning status
 */  
void kpi::set_impact_warning(double d){  
  _impact_warning = d ;
}

/**
 * set impact critical
 *
 *  @param[in]  Set the business impact of a critical status
 */  
void kpi::set_impact_critical(double d){  
  _impact_critical = d ;
}

/**
 * set impact unknown
 *
 *  @param[in]  Set the business impact of an unknown status
 */  
void kpi::set_impact_unknown(double d){  
  _impact_unknown = d;
}

  

