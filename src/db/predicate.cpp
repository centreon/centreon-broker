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
#include "db/db_exception.h"
#include "db/predicate.h"

using namespace CentreonBroker::DB;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 Predicate                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief Predicate default constructor.
 *
 *  Does nothing, as Predicate is just an interface with no data members.
 */
Predicate::Predicate() throw () {}

/**
 *  \brief Predicate copy constructor.
 *
 *  Does nothing, as Predicate is just an interface with no data members.
 */
Predicate::Predicate(const Predicate& predicate) throw ()
{
  (void)predicate;
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Does nothing, as Predicate is just an interface with no data members.
 */
Predicate& Predicate::operator=(const Predicate& predicate) throw ()
{
  (void)predicate;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Predicate destructor.
 *
 *  Does nothing, as Predicate is just an interface with no data members.
 */
Predicate::~Predicate() {}


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
 *  \brief Delete subpredicates.
 *
 *  Clean the object by deleting the two subpredicates.
 */
void And::Clean()
{
  // Delete the left member
  if (this->left_)
    {
      delete (this->left_);
      this->left_ = NULL;
    }

  // Delete the right member
  if (this->right_)
    {
      delete (this->right_);
      this->right_ = NULL;
    }

  return ;
}

/**
 *  \brief Copy data of the given object to the current instance.
 *
 *  Copy data defined within the And class from the given object to the current
 *  instance. This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  \param[in] a_n_d Object to copy data from.
 */
void And::InternalCopy(const And& a_n_d)
{
  this->left_ = a_n_d.left_->Duplicate();
  this->right_ = a_n_d.right_->Duplicate();
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief And constructor.
 *
 *  Initialize the two subpredicates with copies of the arguments.
 *
 *  \param[in] left  First subpredicate (the left member).
 *  \param[in] right Second subpredicate (the right member).
 */
And::And(const Predicate& left, const Predicate& right)
{
  this->left_ = left.Duplicate();
  this->right_ = right.Duplicate();
}

/**
 *  \brief And copy constructor.
 *
 *  Copy the left and right subpredicates of the given object to the current
 *  instance.
 *
 *  \param[in] a_n_d Object to copy predicates from.
 */
And::And(const And& a_n_d) : Predicate(a_n_d)
{
  this->InternalCopy(a_n_d);
}

/**
 *  \brief And destructor.
 *
 *  Delete subpredicates.
 */
And::~And()
{
  this->Clean();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the left and right subpredicates of the given object to the current
 *  instance.
 *
 *  \param[in] a_n_d Object to copy predicates from.
 *
 *  \return *this
 */
And& And::operator=(const And& a_n_d)
{
  this->Predicate::operator=(a_n_d);
  this->InternalCopy(a_n_d);
  return (*this);
}

/**
 *  \brief Accept a visitor.
 *
 *  The goal of this overriden method is to expose the concrete predicate type.
 *  This is done by calling the overloaded method Visit() of PredicateVisitor.
 *
 *  \param[in] visitor Visitor.
 */
void And::Accept(PredicateVisitor& visitor) const
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Duplicate the And predicate.
 *
 *  \return An exact copy of this predicate.
 */
Predicate* And::Duplicate() const
{
  if (!this->left_ || !this->right_)
    throw (DBException(0,
                       DBException::PREDICATE,
                       "Tried to duplicate an incomplete And predicate."));
  return (new And(*this->left_, *this->right_));
}

/**
 *  Get the left member of the predicate.
 *
 *  \return Left member of the predicate.
 */
const Predicate& And::Left() const
{
  return (*this->left_);
}

/**
 *  Get the right member of the predicate.
 *
 *  \return Right member of the predicate.
 */
const Predicate& And::Right() const
{
  return (*this->right_);
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
 *  \brief Delete subpredicates.
 *
 *  Clean the object by deleting the two subpredicates.
 */
void Equal::Clean()
{
  // Delete the left member
  if (this->left_)
    {
      delete (this->left_);
      this->left_ = NULL;
    }

  // Delete the right member
  if (this->right_)
    {
      delete (this->right_);
      this->right_ = NULL;
    }

  return ;
}

/**
 *  \brief Copy data of the given object to the current instance.
 *
 *  Copy data defined within the Equal class from the given object to the
 *  current instance. This method is used by the copy constructor and the
 *  assignment operator.
 *
 *  \param[in] equal Object to copy data from.
 */
void Equal::InternalCopy(const Equal& equal)
{
  this->left_ = equal.left_->Duplicate();
  this->right_ = equal.right_->Duplicate();
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Equal constructor.
 *
 *  Initialize the two subpredicates with copies of the arguments.
 *
 *  \param[in] left First subpredicate (left member).
 *  \param[in] right Second subpredicate (right member).
 */
Equal::Equal(const Predicate& left, const Predicate& right)
{
  this->left_ = left.Duplicate();
  this->right_ = right.Duplicate();
}

/**
 *  \brief Equal copy constructor.
 *
 *  Copy the left and right subpredicates of the given object to the current
 *  instance.
 *
 *  \param[in] equal Object to copy predicates from.
 */
Equal::Equal(const Equal& equal) : Predicate(equal)
{
  this->InternalCopy(equal);
}

/**
 *  \brief Equal destructor.
 *
 *  Delete subpredicates.
 */
Equal::~Equal()
{
  this->Clean();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the left and right subpredicates of the given object to the current
 *  instance.
 *
 *  \param[in] equal Object to copy predicates from.
 *
 *  \return *this
 */
Equal& Equal::operator=(const Equal& equal)
{
  this->Predicate::operator=(equal);
  this->InternalCopy(equal);
  return (*this);
}

/**
 *  \brief Accept a visitor.
 *
 *  The goal of this overriden method is to expose the concrete predicate type.
 *  This is done by calling the overloaded method Visit() of PredicateVisitor.
 *
 *  \param[in] visitor Visitor.
 */
void Equal::Accept(PredicateVisitor& visitor) const
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Duplicates the Equal predicate.
 *
 *  \return An exact copy of this predicate.
 */
Predicate* Equal::Duplicate() const
{
  if (!this->left_ || !this->right_)
    throw (DBException(0,
                       DBException::PREDICATE,
                       "Tried to duplicate an incomplete Equal predicate."));
  return (new Equal(*this->left_, *this->right_));
}

/**
 *  Get the left member of the predicate.
 *
 *  \return Left member of the predicate.
 */
const Predicate& Equal::Left() const
{
  return (*this->left_);
}

/**
 *  Get the right member of the predicate.
 *
 *  \return Right member of the predicate.
 */
const Predicate& Equal::Right() const
{
  return (*this->right_);
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
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Field constructor.
 *
 *  \param[in] field Field name.
 */
Field::Field(const std::string& field) : field_(field) {}

/**
 *  \brief Field copy constructor.
 *
 *  Copy the field name of the given object to the current instance.
 *
 *  \param[in] field Field to copy.
 */
Field::Field(const Field& field) : Predicate(field)
{
  this->field_ = field.field_;
}

/**
 *  Field destructor.
 */
Field::~Field() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the field name of the given object to the current instance.
 *
 *  \param[in] field Field to copy.
 *
 *  \return *this
 */
Field& Field::operator=(const Field& field)
{
  this->Predicate::operator=(field);
  this->field_ = field.field_;
  return (*this);
}

/**
 *  \brief Accept a visitor.
 *
 *  The goal of this overriden method is to expose the concrete predicate type.
 *  This is done by calling the overloaded method Visit() of PredicateVisitor.
 *
 *  \param[in] visitor Visitor.
 */
void Field::Accept(PredicateVisitor& visitor) const
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Duplicate a Field.
 *
 *  \return An exact copy of this predicate.
 */
Predicate* Field::Duplicate() const
{
  return (new Field(this->field_));
}

/**
 *  Get the field name.
 *
 *  \return The name of the field.
 */
const std::string& Field::Name() const throw ()
{
  return (this->field_);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               Placeholder                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  \brief Placeholder default constructor.
 *
 *  Does nothing, as Placeholder does not contain any data.
 */
Placeholder::Placeholder() throw () {}

/**
 *  \brief Placeholder copy constructor.
 *
 *  Does nothing, as Placeholder does not contain any data.
 *
 *  \param[in] ph Object to copy from.
 */
Placeholder::Placeholder(const Placeholder& ph) throw () : Predicate(ph) {}

/**
 *  \brief Placeholder destructor.
 *
 *  Does nothing, as Placeholder does not contain any data.
 */
Placeholder::~Placeholder() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Does nothing, as Placeholder does not contain any data.
 *
 *  \param[in] ph Object to copy from.
 *
 *  \return *this
 */
Placeholder& Placeholder::operator=(const Placeholder& ph) throw ()
{
  this->Predicate::operator=(ph);
  return (*this);
}

/**
 *  \brief Accept a visitor.
 *
 *  The goal of this overriden method is to expose the concrete predicate type.
 *  This is done by calling the overloaded method Visit of PredicateVisitor.
 *
 *  \param[in] visitor Visitor.
 */
void Placeholder::Accept(PredicateVisitor& visitor) const
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Duplicate the Placeholder.
 *
 *  \return A new Placeholder object.
 */
Predicate* Placeholder::Duplicate() const
{
  return (new Placeholder);
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
 *  \brief Copy data of the given object to the current instance.
 *
 *  Copy data defined within the Terminal class from the given object to the
 *  current instance. This method is used by the copy constructor and the
 *  assignment operator.
 *
 *  \param[in] terminal Object to copy data from.
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
 *  Build a Terminal from a bool.
 *
 *  \param[in] b Value.
 */
Terminal::Terminal(bool b) throw () : type_('b')
{
  this->value_.b = b;
}

/**
 *  Build a Terminal from a double.
 *
 *  \param[in] d Value.
 */
Terminal::Terminal(double d) throw () : type_('d')
{
  this->value_.d = d;
}

/**
 *  Build a Terminal from an int.
 *
 *  \param[in] i Value.
 */
Terminal::Terminal(int i) throw () : type_('i')
{
  this->value_.i = i;
}

/**
 *  Build a Terminal from a short.
 *
 *  \param[in] s Value.
 */
Terminal::Terminal(short s) throw () : type_('s')
{
  this->value_.s = s;
}

/**
 *  Build a Terminal from a string.
 *
 *  \param[in] S Value.
 */
Terminal::Terminal(const char* S) throw () : type_('S')
{
  this->value_.S = S;
}

/**
 *  Build a Terminal from a time.
 *
 *  \param[in] t Value.
 */
Terminal::Terminal(time_t t) throw () : type_('t')
{
  this->value_.t = t;
}

/**
 *  \brief Terminal copy constructor.
 *
 *  Copy the value stored within the given Terminal to the current instance.
 *
 *  \param[in] terminal Object to copy value from.
 */
Terminal::Terminal(const Terminal& terminal) throw () : Predicate(terminal)
{
  this->InternalCopy(terminal);
}

/**
 *  \brief Terminal destructor.
 *
 *  Does nothing.
 */
Terminal::~Terminal() throw () {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the value stored within the given Terminal to the current instance.
 *
 *  \param[in] terminal Object to copy value from.
 *
 *  \return *this
 */
Terminal& Terminal::operator=(const Terminal& terminal) throw ()
{
  this->Predicate::operator=(terminal);
  this->InternalCopy(terminal);
  return (*this);
}

/**
 *  \brief Accept a visitor.
 *
 *  The goal of this overriden method is to expose the concrete predicate type.
 *  This is done by calling the overloaded method Visit() of PredicateVisitor.
 *
 *  \param[in] visitor Visitor.
 */
void Terminal::Accept(PredicateVisitor& visitor) const
{
  visitor.Visit(*this);
  return ;
}

/**
 *  Duplicate the Terminal.
 *
 *  \return An exact copy of this terminal.
 */
Predicate* Terminal::Duplicate() const
{
  Predicate* ret;

  switch (this->type_)
    {
     case 'b':
      ret = new Terminal(this->value_.b);
      break ;
     case 'd':
      ret = new Terminal(this->value_.d);
      break ;
     case 'i':
      ret = new Terminal(this->value_.i);
      break ;
     case 's':
      ret = new Terminal(this->value_.s);
      break ;
     case 'S':
      ret = new Terminal(this->value_.S);
      break ;
     case 't':
      ret = new Terminal(this->value_.t);
      break ;
    default:
      throw (DBException(this->type_,
                         DBException::PREDICATE,
                         "Tried to duplicate an invalid Terminal."));
    }
  return (ret);
}

/**
 *  Get the boolean stored within the object.
 *
 *  \return The boolean stored within the object.
 */
bool Terminal::GetBool() const throw ()
{
  return (this->value_.b);
}

/**
 *  Get the double stored within the object.
 *
 *  \return The double stored within the object.
 */
double Terminal::GetDouble() const throw ()
{
  return (this->value_.d);
}

/**
 *  Get the integer stored within the object.
 *
 *  \return The integer stored within the object.
 */
int Terminal::GetInt() const throw ()
{
  return (this->value_.i);
}

/**
 *  Get the short stored within the object.
 *
 *  \return The short stored within the object.
 */
short Terminal::GetShort() const throw ()
{
  return (this->value_.s);
}

/**
 *  Get the string stored within the object.
 *
 *  \return The string stored within the object.
 */
const char* Terminal::GetString() const throw ()
{
  return (this->value_.S);
}

/**
 *  Get the time stored within the object.
 *
 *  \return The time stored within the object.
 */
time_t Terminal::GetTime() const throw ()
{
  return (this->value_.t);
}

/**
 *  \brief Get the type of the value stored within the Terminal.
 *
 *  Valid values are :
 *
 *    - 'b' for bool
 *    - 'd' for double
 *    - 'i' for int
 *    - 's' for short
 *    - 'S' for string (const char*)
 *    - 't' for time (time_t)
 *
 *  \return The type of the value stored within the Terminal.
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

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief PredicateVisitor default constructor.
 *
 *  Does nothing, as PredicateVisitor does not contain any data member.
 */
PredicateVisitor::PredicateVisitor() throw () {}

/**
 *  \brief PredicateVisitor copy constructor.
 *
 *  Does nothing, as PredicateVisitor does not contain any data member.
 *
 *  \param[in] pv Unused.
 */
PredicateVisitor::PredicateVisitor(const PredicateVisitor& pv) throw ()
{
  (void)pv;
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Does nothing, as PredicateVisitor does not contain any data member.
 *
 *  \param[in] pv Unused.
 *
 *  \return *this
 */
PredicateVisitor& PredicateVisitor::operator=(const PredicateVisitor& pv)
  throw ()
{
  (void)pv;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PredicateVisitor destructor.
 *
 *  Does nothing, as PredicateVisitor does not contain any data member.
 */
PredicateVisitor::~PredicateVisitor() {}
