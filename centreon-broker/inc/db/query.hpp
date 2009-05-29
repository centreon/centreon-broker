/*
** query.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 05/29/09 Matthieu Kermagoret
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
    int                        arg;
    const Mapping<ObjectType>& default_mapping_;
    Mapping<ObjectType>*       modified_mapping_;

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
      : default_mapping_(mapping), modified_mapping_(NULL) {}

    /**
     *  Query destructor.
     */
    virtual      ~Query()
    {
      if (this->modified_mapping_)
	delete (this->modified_mapping_);
    }

    /**
     *  Delete a field from the mapping. This will force to copy the default
     *  mapping.
     */
    Query&       operator>>(const std::string& field)
    {
      if (!this->modified_mapping_)
	this->modified_mapping_ = new Mapping<ObjectType>(this->mapping_);
      this->modified_mapping_ >> field;
      return (*this);
    }

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
      decltype(this->default_mapping_.setters_.begin()) it;
      decltype(this->default_mapping_.setters_.end()) end_it;

      if (this->modified_mapping_)
	{
	  it = this->modified_mapping_->setters_.begin();
	  end_it = this->modified_mapping_->setters.end();
	}
      else
	{
	  it = this->default_mapping_.setters.begin();
	  end_it = this->default_mapping_.setters.end();
	}
      for (this->arg = 1; it != end_it; this->arg_++)
	((*it).second)(query, obj);
      return ;
    }
  };
};

#endif /* !DB_QUERY_HPP_ */
