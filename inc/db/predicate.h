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

# include <boost/smart_ptr.hpp>
# include <string>

namespace          CentreonBroker
{
  namespace        DB
  {
    // This class is defined later.
    class          PredicateVisitor;

    /**
     *  This is the root class of all predicates.
     */
    class          Predicate
    {
     public:
                   Predicate() throw ();
                   Predicate(const Predicate& predicate) throw ();
      virtual      ~Predicate();
      Predicate&   operator=(const Predicate& predicate) throw ();
      virtual void Accept(PredicateVisitor& visitor) = 0;
    };

    /**
     *  Determines whether or not both predicates are true.
     */
    class                          And : public Predicate
    {
     private:
      boost::shared_ptr<Predicate> p1_;
      boost::shared_ptr<Predicate> p2_;
      void                         InternalCopy(const And& a_n_d);

     public:
      /**
       *  And default constructor.
       */
      template                     <typename Predicate1, typename Predicate2>
                                   And(const Predicate1& p1,
                                       const Predicate2& p2)
	: p1_(new Predicate1(p1)), p2_(new Predicate2(p2)) {}

                                   And(const And& a_n_d);
                                   ~And();
      And&                         operator=(const And& a_n_d);
      void                         Accept(PredicateVisitor& visitor);
      Predicate&                   Left();
      Predicate&                   Right();
    };

    /**
     *  Verify the equality between two members. This predicate is a
     *  composition of two other predicates.
     */
    class                          Equal : public Predicate
    {
     private:
      Predicate*                   p1_;
      Predicate*                   p2_;
      void                         InternalCopy(const Equal& equal);

     public:
      /**
       *  Equal constructor.
       */
      template                     <typename Predicate1, typename Predicate2>
                                   Equal(const Predicate1& p1,
                                         const Predicate2& p2)
	: p1_(new Predicate1(p1)), p2_(new Predicate2(p2)) {}

                                   Equal(const Equal& equal);
                                   ~Equal();
      Equal&                       operator=(const Equal& equal);
      void                         Accept(PredicateVisitor& visitor);
      Predicate&                   Left();
      Predicate&                   Right();
    };

    /**
     *  This will match a field.
     */
    class                Field : public Predicate
    {
     private:
      std::string        field_;
      void               InternalCopy(const Field& field);

     public:
                         Field(const std::string& field);
                         Field(const Field& field);
                         ~Field();
      Field&             operator=(const Field& field);
      void               Accept(PredicateVisitor& visitor);
      const std::string& GetName() const throw ();
    };

    /**
     *  Terminal predicate. This can either be a double, an int or some other
     *  base type.
     */
    class           Terminal : public Predicate
    {
      char          type_;
      union
      {
	double      d;
	int         i;
	short       s;
	const char* S;
	time_t      t;
      }             value_;
      void          InternalCopy(const Terminal& terminal) throw ();

     public:
                    Terminal(double d) throw ();
                    Terminal(int i) throw ();
                    Terminal(short s) throw ();
                    Terminal(const char* S) throw ();
                    Terminal(time_t t) throw ();
                    Terminal(const Terminal& terminal) throw ();
                    ~Terminal() throw ();
      Terminal&     operator=(const Terminal& terminal) throw ();
      void          Accept(PredicateVisitor& visitor);
      double        GetDouble() const throw ();
      int           GetInt() const throw ();
      short         GetShort() const throw ();
      const char*   GetString() const throw ();
      time_t        GetTime() const throw ();
      char          GetType() const throw ();
    };

    /**
     *  This class will be subclassed by those who needs to unroll predicates.
     */
    class               PredicateVisitor
    {
     public:
                        PredicateVisitor();
                        PredicateVisitor(const PredicateVisitor& pv);
      virtual           ~PredicateVisitor();
      PredicateVisitor& operator=(const PredicateVisitor& pv);
      virtual void      Visit(And& a_n_d) = 0;
      virtual void      Visit(Equal& equal) = 0;
      virtual void      Visit(Field& field) = 0;
      virtual void      Visit(Terminal& terminal) = 0;
    };
  }
}

#endif /* !DB_PREDICATE_H_ */
