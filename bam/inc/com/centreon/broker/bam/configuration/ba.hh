/*
** Copyright 2009-2014 Merethis
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
#ifndef CCBN_BA_HH
#  define CCBN_BA_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace configuration{
  
  class ba {
    
  public:
    /**   
     *  Constructor
     */
    ba( unsigned int id,
	const std::string& name,
	double level,
	double warning_level,
	double critical_level);
    
    unsigned int get_id() const;
    std::string const& get_name()const;
    double get_level()const;
    double get_warning_level()const;
    double get_critical_level()const;
    
    void set_id( unsigned int id);
    void set_name ( const std::string& name);
    void set_level ( double level);
    void set_warning_level ( double warning_level);  
    void set_critical_level ( double critical_level);
    
  private:
    unsigned int _id;
    std::string _name;
    double _level;
    double _warning_level;
    double _critical_level;
    
  };
  
}

CCB_END()

#endif
