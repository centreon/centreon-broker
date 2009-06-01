/*
** query.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 06/01/09 Matthieu Kermagoret
*/

#ifndef DB_QUERY_HPP_
# define DB_QUERY_HPP_

# include "db/mapping.hpp"

namespace CentreonBroker
{
  /**
   *  This template class is the base of all queries that will be executed.
   */
  template <typename ObjectType>
  class          Query
  {
   protected:
    int                        arg_;
    const Mapping<ObjectType>& mapping_;

    /**
     *  Query copy constructor.
     */
                 Query(const Query& query)
    {
      (void)query;
    }

    /**
     *  Query operator= overload.
     */
    Query&       operator=(const Query& query)
    {
      (void)query;
      return (*this);
    }

   public:
    /**
     *  Query constructor.
     */
                 Query(const Mapping<ObjectType>& mapping)
      : mapping_(mapping) {}

    /**
     *  Query destructor.
     */
    virtual      ~Query() {}

    /**
     *  Execute the query.
     */
    virtual void Execute(const ObjectType& object) = 0;

    /**
     *  Prepare the query.
     */
    virtual void Prepare() = 0;

    /**
     *  Set the current argument to be a double.
     */
    virtual void SetDouble(double d) = 0;

    /**
     *  Set the current argument to be an int.
     */
    virtual void SetInt(int i) = 0;

    /**
     *  Set the current argument to be a short.
     */
    virtual void SetShort(short s) = 0;

    /**
     *  Set the current argument to be a string.
     */
    virtual void SetString(const std::string& s) = 0;

    /**
     *  Set the current argument to be a time_t.
     */
    virtual void SetTimeT(time_t t) = 0;

    /**
     *  Visit the object and call appropriates SetXXX methods.
     */
    void         VisitObject(Query<ObjectType>* query, const ObjectType& obj)
    {
      // XXX : does this really works ?
      decltype(this->mapping_.setters_.begin()) it;
      decltype(this->mapping_.setters_.end()) end_it;

      it = this->mapping_.setters_.begin();
      end_it = this->mapping_.setters_.end();
      for (this->arg_ = 1; it != end_it; this->arg_++)
	(*((*it).second))(query, obj);
      return ;
    }
  };
}

#endif /* !DB_QUERY_HPP_ */
