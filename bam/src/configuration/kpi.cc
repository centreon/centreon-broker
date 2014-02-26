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

using namespace com::centreon::broker::configuration;

    
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
   *
   */  
  unsigned kpi::get_id()const{  
    return _id;
  }

  /**
   *
   */  
  short kpi::get_state_type()const{
    return  _state_type ;
  }

  /**
   *
   */  
  unsigned int kpi::get_host_id()const{
    return   _host_id;
  }

  /**
   *
   */  
  unsigned int kpi::get_service_id()const{ 
    return _service_id;
  }

  /**
   *
   */  
  bool kpi::is_service() const { 
    return (_service_id != 0 ) ; 
  }  

  /**
   *
   */  
  bool kpi::is_ba() const { 
    return (_ba_id != 0); 
  }

  /**
   *
   */  
  unsigned int kpi::get_ba_id()const{
    return  _ba_id ;
  }

  /**
   *
   */  
  short kpi::get_status()const{
    return  _status ;
  }

  /**
   *
   */  
  short kpi::get_last_hard_state()const{
    return  _last_hard_state;
  }

  /**
   *
   */  
  bool kpi::is_downtimed() const{  
    return _downtimed;
  }

  /**
   *
   */  
  bool kpi::is_acknowledged() const{  
    return _acknowledged;
  }

  /**
   *
   */  
  bool kpi::ignore_downtime() const{  
    return _ignore_downtime;
  }

  /**
   *
   */  
  bool kpi::ignore_acknowledgement() const{  
    return _ignore_acknowledgement;
  }
  
  /**
   *
   */  
  double kpi::get_impact_warning() const{  
    return _impact_warning;
  }

  /**
   *
   */  
  double kpi::get_impact_critical() const{  
    return _impact_critical;
  }

  /**
   *
   */  
  double kpi::get_impact_unknown() const{  
   return _impact_unknown;
  }

  /**
   *
   */  
  void kpi::set_state_type( short s){  
    _state_type = s ;
  }
    
  /**
   *
   */  
  void kpi::set_host_id( unsigned int i){  
    _host_id = i;
  }

  /**
   *
   */  
  void kpi::set_service_id(unsigned int i ){  
    _service_id = i;
  }

  /**
   *
   */  
  void kpi::set_ba_id(unsigned int i){  
    _ba_id = i;
  }

  /**
   *
   */  
  void kpi::set_status(short s){  
    _status = s;
  }

  /**
   *
   */  
  void kpi::set_last_hard_state(short s){  
    _last_hard_state = s;
  }

  /**
   *
   */  
  void kpi::set_downtimed(bool b){  
   _downtimed = b ;
  }

  /**
   *
   */  
  void kpi::set_acknowledged(bool b){  
    _acknowledged = b;
  }

  /**
   *
   */  
  void kpi::ignore_downtime( bool b){  
   _ignore_downtime = b ;
  }

  /**
   *
   */  
  void kpi::ignore_acknowledgement(bool b){  
   _ignore_acknowledgement = b ;
  }

  /**
   *
   */  
  void kpi::set_impact_warning(double d){  
   _impact_warning = d ;
  }

  /**
   *
   */  
  void kpi::set_impact_critical(double d){  
   _impact_critical = d ;
  }

  /**
   *
   */  
  void kpi::set_impact_unknown(double d){  
    _impact_unknown = d;
  }

  

