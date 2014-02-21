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

CCB_BEGIN()



class  bool_expression{

  unsigned int  _id;
  double        _impact;
  std::string   _expression;
  bool          _impact_if;
  bool          _state; 


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

  unsigned int         getId()const         { return _id;}
  double               getImpact()const     { return _impact;}
  const std::string&   getExpression()const { return _expression;}
  bool                 getImpactIf() const  { return _impact_if; }
  bool                 getState()const      { return _state; }

  // void setId(  unsigned int)const         { return _id;}
  void                 setImpact(double d)                  {  _impact=d;}
  void                 setExpression(const std::string& s)  {  _expression = s;}
  void                 setImpactIf( bool b)                 {  _impact_if = b; }
  void                 setState(bool s)                     {  _state = s; }

};





CCB_END()

#endif
