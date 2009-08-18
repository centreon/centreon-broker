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

#ifndef DB_PREDICATE_H_
# define DB_PREDICATE_H_

# include <string>

namespace          CentreonBroker
{
  namespace        DB
  {
    // This class is defined at the bottom of this file.
    class          PredicateVisitor;

    /**
     *  \class Predicate predicate.h "db/predicate.h"
     *  \brief Root of all predicate element of an SQL query.
     *
     *  A predicate object is a condition that can be checked within a SQL
     *  query and put on the WHERE part of the query. Predicate is an interface
     *  defining a way to expand a Predicate object to its corresponding SQL
     *  string. This method uses the Visitor design pattern : the Predicate can
     *  virtually Accept() a visitor and expose to him the true Predicate type
     *  through the visitor's overloaded Visit() method.
     *
     *  \see PredicateVisitor
     */
    class          Predicate
    {
     protected:
                         Predicate() throw ();
                         Predicate(const Predicate& predicate) throw ();
      Predicate&         operator=(const Predicate& predicate) throw ();

     public:
      virtual            ~Predicate();
      virtual void       Accept(PredicateVisitor& visitor) const = 0;
      virtual Predicate* Duplicate() const = 0;
    };

    /**
     *  \class And predicate.h "db/predicate.h"
     *  \brief Determines whether or not both subpredicates are true.
     *
     *  This predicate is used to determine whether or not the two
     *  subpredicates are true.
     */
    class              And : public Predicate
    {
     private:
      Predicate*       left_;
      Predicate*       right_;
      void             Clean();
      void             InternalCopy(const And& a_n_d);

     public:
                       And(const Predicate& left, const Predicate& right);
                       And(const And& a_n_d);
                       ~And();
      And&             operator=(const And& a_n_d);
      void             Accept(PredicateVisitor& visitor) const;
      Predicate*       Duplicate() const;
      const Predicate& Left() const;
      const Predicate& Right() const;
    };

    /**
     *  \class Equal predicate.h "db/predicate.h"
     *  \brief Determines whether or not both subpredicates are equal.
     *
     *  This predicate is used to determine whether or not the two
     *  subpredicates are equal.
     */
    class              Equal : public Predicate
    {
     private:
      Predicate*       left_;
      Predicate*       right_;
      void             Clean();
      void             InternalCopy(const Equal& equal);

     public:
                       Equal(const Predicate& left, const Predicate& right);
                       Equal(const Equal& equal);
                       ~Equal();
      Equal&           operator=(const Equal& equal);
      void             Accept(PredicateVisitor& visitor) const;
      Predicate*       Duplicate() const;
      const Predicate& Left() const;
      const Predicate& Right() const;
    };

    /**
     *  \class Field predicate.h "db/predicate.h"
     *  \brief Expand to a table field.
     *
     *  When used inside a composed predicate, objects of this class maps to
     *  the actual table field.
     */
    class                Field : public Predicate
    {
     private:
      std::string        field_;

     public:
                         Field(const std::string& field);
                         Field(const Field& field);
                         ~Field();
      Field&             operator=(const Field& field);
      void               Accept(PredicateVisitor& visitor) const;
      Predicate*         Duplicate() const;
      const std::string& Name() const throw ();
    };

    /**
     *  \class Placeholder predicate.h "db/predicate.h"
     *  \brief A parameter that can be set later.
     *
     *  A placeholder is a parameter that can be set later in the query. Such
     *  objects are used on prepared queries where predicate parameters can
     *  change on every execution.
     */
    class          Placeholder : public Predicate
    {
     public:
                   Placeholder() throw ();
                   Placeholder(const Placeholder& ph) throw ();
                   ~Placeholder();
      Placeholder& operator=(const Placeholder& ph) throw ();
      void         Accept(PredicateVisitor& visitor) const;
      Predicate*   Duplicate() const;
    };

    /**
     *  \class Terminal predicate.h "db/predicate.h"
     *  \brief Explicit value.
     *
     *  A terminal is a determined value. Unlike Placeholder, its value cannot
     *  be changed later. This can be any kind of constant or string.
     */
    class           Terminal : public Predicate
    {
      char          type_;
      union
      {
	bool        b;
	double      d;
	int         i;
	short       s;
	const char* S;
	time_t      t;
      }             value_;
      void          InternalCopy(const Terminal& terminal) throw ();

     public:
                    Terminal(bool b) throw ();
                    Terminal(double d) throw ();
                    Terminal(int i) throw ();
                    Terminal(short s) throw ();
                    Terminal(const char* S) throw ();
                    Terminal(time_t t) throw ();
                    Terminal(const Terminal& terminal) throw ();
                    ~Terminal() throw ();
      Terminal&     operator=(const Terminal& terminal) throw ();
      void          Accept(PredicateVisitor& visitor) const;
      Predicate*    Duplicate() const;
      bool          GetBool() const throw ();
      double        GetDouble() const throw ();
      int           GetInt() const throw ();
      short         GetShort() const throw ();
      const char*   GetString() const throw ();
      time_t        GetTime() const throw ();
      char          GetType() const throw ();
    };

    /**
     *  \class PredicateVisitor predicate.h "db/predicate.h"
     *  \brief Visit a generic predicate.
     *
     *  A class willing to expand a generic predicate object to its underlying
     *  concrete objects can subclass PredicateVisitor. The Visitor design
     *  pattern is used to expose the concrete type of any predicate. Just call
     *  Accept() on the predicate object with the appropriate visitor and
     *  override the Visit() methods to have full access to any predicate
     *  object.
     */
    class               PredicateVisitor
    {
     protected:
                        PredicateVisitor() throw ();
                        PredicateVisitor(const PredicateVisitor& pv) throw ();
      PredicateVisitor& operator=(const PredicateVisitor& pv) throw ();

     public:
      virtual           ~PredicateVisitor();
      virtual void      Visit(const And& a_n_d) = 0;
      virtual void      Visit(const Equal& equal) = 0;
      virtual void      Visit(const Field& field) = 0;
      virtual void      Visit(const Placeholder& placeholder) = 0;
      virtual void      Visit(const Terminal& terminal) = 0;
    };
  }
}

#endif /* !DB_PREDICATE_H_ */
