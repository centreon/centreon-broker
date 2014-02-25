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
#ifndef CCB_CONFIGURATION_BOOL_EXPRESSION_HH
#  define CCB_CONFIGURATION_BOOL_EXPRESSION_HH
#  include <string>

CCB_BEGIN()

class  bool_expression {
public:  
  bool_expression(     
    unsigned int id,
    double impact,
    std::string const& expression,
    bool impact_if,
    bool state  );

  unsigned int get_id() const;
  double get_impact() const;  
  std::string const& get_expression() const; 
  bool get_impactIf() const;   
  bool get_state() const;     

  void set_impact( double d);  
  void set_expression( std::string const & s);
  void set_impactIf( bool b); 
  void set_state( bool s); 

private:
  unsigned int  _id;
  double        _impact;
  std::string   _expression;
  bool          _impact_if;
  bool          _state; 
};

CCB_END()

#endif
