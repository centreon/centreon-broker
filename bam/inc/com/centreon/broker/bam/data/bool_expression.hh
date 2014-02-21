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
#ifndef CCB_BOOL_EXPRESSION_HH
# define CCB_BOOL_EXPRESSION_HH

# include "com/centreon/broker/namespace.hh"
# include <string>
namespace com { 
 namespace centreon { 
  namespace broker {
   namespace configuration {



class  bool_expression{



  // friend void swap(  bool_expression& left,  bool_expression& right) throw();


public:
  // COMPILER GENERATED CODE IS OK
  // bool_expression( const bool_expression& right);
  //bool_expression& operator= ( bool_expression right);

  bool_expression( 
		  unsigned int         id,
		  double               impact,
		  const std::string&   expression,
		  bool                 impact_if,
		  bool                 state 
		   ):
    _id(id),
    _impact(impact),
    _expression(expression),
    _impact_if(impact_if),
    _state(state) 
 {}

  unsigned int         get_id()const         { return _id;}
  double               get_impact()const     { return _impact;}
  const std::string&   get_expression()const { return _expression;}
  bool                 get_impactIf() const  { return _impact_if; }
  bool                 get_state()const      { return _state; }

  // void setId(  unsigned int)const         { return _id;}
  void                 set_impact(double d)                  {  _impact=d;}
  void                 set_expression(const std::string& s)  {  _expression = s;}
  void                 set_impactIf( bool b)                 {  _impact_if = b; }
  void                 set_state(bool s)                     {  _state = s; }

private:

  unsigned int  _id;
  double        _impact;
  std::string   _expression;
  bool          _impact_if;
  bool          _state; 


};





   }
  }
 }
}

#endif
