/*
** mysql_insert.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 05/30/09 Matthieu Kermagoret
*/

#ifndef MYSQL_INSERT_HPP_
# define MYSQL_INSERT_HPP_

# include <mysql_connection.h>
# include <mysql_prepared_statement.h>
# include "db/insert.hpp"

namespace   CentreonBroker
{
  /**
   *  This template encapsulates a MySQL INSERT query.
   */
  template  <typename ObjectType>
  class     MySQLInsert : public Insert<ObjectType>
  {
   private:
    sql::Connection* myconn_;
    sql::PreparedStatement* stmt_;
    /**
     *  MySQLInsert copy constructor.
     */
            MySQLInsert(const MySQLInsert& myinsert)
      : Insert<ObjectType>(myinsert.mapping_)
    {
      (void)myinsert;
    }

    /**
     *  MySQLInsert operator= overload.
     */
    MySQLInsert& operator=(const MySQLInsert& myinsert)
    {
      (void)myinsert;
      return (*this);
    }

   public:
    /**
     *  MySQLInsert default constructor.
     */
            MySQLInsert(const Mapping<ObjectType>& mapping,
                        sql::Connection* myconn)
      : Insert<ObjectType>(mapping), myconn_(myconn), stmt_(NULL) {}

    /**
     *  MySQLInsert destructor.
     */
    virtual ~MySQLInsert()
    {
      if (this->stmt_)
	delete (this->stmt_);
    }

    /**
     *  Execute the query.
     */
    void    Execute(const ObjectType& object)
    {
      this->VisitObject(this, object);
      this->stmt_->execute();
      return ;
    }

    /**
     *  Prepare the query.
     */
    void    Prepare()
    {
      decltype(this->default_mapping_.setters_.begin()) it;
      decltype(this->default_mapping_.setters_.end()) it_end;
      std::string query;

      query = "INSERT INTO ";
      query += (this->modified_mapping_ ? this->modified_mapping_->table_
                                        : this->default_mapping_.table_);
      query += " SET ";
      if (this->modified_mapping_)
	{
	  it = this->modified_mapping_->setters_.begin();
	  it_end = this->modified_mapping_->setters_.end();
	}
      else
	{
	  it = this->default_mapping_.setters_.begin();
	  it_end = this->default_mapping_.setters_.end();
	}
      while (it != it_end)
	{
	  query += (*it).first;
	  query += "=?, ";
	  it++;
	}
      query.resize(query.size() - 2);
      this->stmt_ = this->myconn_->prepareStatement(query);
      return ;
    }

    /**
     *  Set the current argument to be a double.
     */
    void    SetDouble(double d)
    {
      this->stmt_->setDouble(this->arg_, d);
      return ;
    }

    /**
     *  Set the current argument to be an int.
     */
    void    SetInt(int i)
    {
      this->stmt_->setInt(this->arg_, i);
      return ;
    }

    /**
     *  Set the current argument to be a short.
     */
    void    SetShort(short s)
    {
      // XXX : setShort
      this->stmt_->setInt(this->arg_, s);
      return ;
    }

    /**
     *  Set the current argument to be a string.
     */
    void    SetString(const std::string& s)
    {
      this->stmt_->setString(this->arg_, s);
      return ;
    }

    /**
     *  Set the current argument to be a time_t.
     */
    void    SetTimeT(time_t t)
    {
      this->stmt_->setInt(this->arg_, t);
      return ;
    }
  };
}

#endif /* !MYSQL_INSERT_HPP_ */
