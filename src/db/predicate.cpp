/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <cstring>
#include "db/predicate.h"

using namespace CentreonBroker::DB;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 Predicate                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  Predicate default constructor.
 */
Predicate::Predicate() throw ()
{
}

/**
 *  Predicate copy constructor.
 */
Predicate::Predicate(const Predicate& predicate) throw ()
{
  (void)predicate;
}

/**
 *  Predicate destructor.
 */
Predicate::~Predicate()
{
}

/**
 *  Predicate operator= overload.
 */
Predicate& Predicate::operator=(const Predicate& predicate) throw ()
{
  (void)predicate;
  return (*this);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                   And                                       *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void And::InternalCopy(const And& a_n_d)
{
  this->p1_ = a_n_d.p1_;
  this->p2_ = a_n_d.p2_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  And copy constructor.
 */
And::And(const And& a_n_d) : Predicate(a_n_d)
{
  this->InternalCopy(a_n_d);
}

/**
 *  And destructor.
 */
And::~And()
{
}

/**
 *  And operator= overload.
 */
And& And::operator=(const And& a_n_d)
{
  this->Predicate::operator=(a_n_d);
  this->InternalCopy(a_n_d);
  return (*this);
}

/**
 *  Accept a visitor and show him around.
 */
void And::Accept(PredicateVisitor& visitor)
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Returns the left member of the and predicate.
 */
Predicate& And::Left()
{
  return (*this->p1_.get());
}

/**
 *  Returns the right member of the and predicate.
 */
Predicate& And::Right()
{
  return (*this->p2_.get());
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                   Equal                                     *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Equal::InternalCopy(const Equal& equal)
{
  this->p1_ = equal.p1_;
  this->p2_ = equal.p2_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Equal copy constructor.
 */
Equal::Equal(const Equal& equal) : Predicate(equal)
{
  this->InternalCopy(equal);
}

/**
 *  Equal destructor.
 */
Equal::~Equal()
{
}

/**
 *  Equal operator= overload.
 */
Equal& Equal::operator=(const Equal& equal)
{
  this->Predicate::operator=(equal);
  this->InternalCopy(equal);
  return (*this);
}

/**
 *  Show to the visitor the true type of the object.
 */
void Equal::Accept(PredicateVisitor& visitor)
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Returns the left member of the equality.
 */
Predicate& Equal::Left()
{
  return (*this->p1_);
}

/**
 *  Returns the right member of the equality.
 */
Predicate& Equal::Right()
{
  return (*this->p2_);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                Field                                        *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Field::InternalCopy(const Field& field)
{
  this->field_ = field.field_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Field default constructor.
 */
Field::Field(const std::string& field) : field_(field)
{
}

/**
 *  Field copy constructor.
 */
Field::Field(const Field& field) : Predicate(field)
{
  this->InternalCopy(field);
}

/**
 *  Field destructor.
 */
Field::~Field()
{
}

/**
 *  Field operator= overload.
 */
Field& Field::operator=(const Field& field)
{
  this->Predicate::operator=(field);
  this->InternalCopy(field);
  return (*this);
}

/**
 *  Show the true type of the object to the visitor.
 */
void Field::Accept(PredicateVisitor& visitor)
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Returns the name of the field associated to the predicate.
 */
const std::string& Field::GetName() const throw ()
{
  return (this->field_);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                Terminal                                     *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Terminal::InternalCopy(const Terminal& terminal) throw ()
{
  this->type_ = terminal.type_;
  memcpy(&this->value_, &terminal.value_, sizeof(this->value_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Build a Terminal from a double.
 */
Terminal::Terminal(double d) throw () : type_('d')
{
  this->value_.d = d;
}

/**
 *  Build a Terminal from an int.
 */
Terminal::Terminal(int i) throw () : type_('i')
{
  this->value_.i = i;
}

/**
 *  Build a Terminal from a short.
 */
Terminal::Terminal(short s) throw () : type_('s')
{
  this->value_.s = s;
}

/**
 *  Build a Terminal from a string.
 */
Terminal::Terminal(const char* S) throw () : type_('S')
{
  this->value_.S = S;
}

/**
 *  Build a Terminal from a time.
 */
Terminal::Terminal(time_t t) throw () : type_('t')
{
  this->value_.t = t;
}

/**
 *  Terminal copy constructor.
 */
Terminal::Terminal(const Terminal& terminal) throw () : Predicate(terminal)
{
  this->InternalCopy(terminal);
}

/**
 *  Terminal destructor.
 */
Terminal::~Terminal() throw ()
{
}

/**
 *  Terminal operator= overload.
 */
Terminal& Terminal::operator=(const Terminal& terminal) throw ()
{
  this->Predicate::operator=(terminal);
  this->InternalCopy(terminal);
  return (*this);
}

/**
 *  Make a tour of the instance to the visitor.
 */
void Terminal::Accept(PredicateVisitor& visitor)
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Return the double contained within the object.
 */
double Terminal::GetDouble() const throw ()
{
  return (this->value_.d);
}

/**
 *  Return the int contained within the object.
 */
int Terminal::GetInt() const throw ()
{
  return (this->value_.i);
}

/**
 *  Return the short contained within the object.
 */
short Terminal::GetShort() const throw ()
{
  return (this->value_.s);
}

/**
 *  Return the string contained within the object.
 */
const char* Terminal::GetString() const throw ()
{
  return (this->value_.S);
}

/**
 *  Return the string contained within the object.
 */
time_t Terminal::GetTime() const throw ()
{
  return (this->value_.t);
}

/**
 *  Return the type of the object contained within this Predicate.
 */
char Terminal::GetType() const throw ()
{
  return (this->type_);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                              PredicateVisitor                               *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  PredicateVisitor default constructor.
 */
PredicateVisitor::PredicateVisitor()
{
}

/**
 *  PredicateVisitor copy constructor.
 */
PredicateVisitor::PredicateVisitor(const PredicateVisitor& pv)
{
  (void)pv;
}

/**
 *  PredicateVisitor destructor.
 */
PredicateVisitor::~PredicateVisitor()
{
}

/**
 *  PredicateVisitor operator= overload.
 */
PredicateVisitor& PredicateVisitor::operator=(const PredicateVisitor& pv)
{
  (void)pv;
  return (*this);
}
