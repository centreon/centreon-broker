/*
** mysql_update.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/01/09 Matthieu Kermagoret
** Last update 06/01/09 Matthieu Kermagoret
*/

#ifndef MYSQL_UPDATE_HPP_
# define MYSQL_UPDATE_HPP_

# include <mysql_connection.h>
# include <mysql_prepared_statement.h>
# include "db/update.hpp"

namespace   CentreonBroker
{
  /**
   *  This template encapsulates a MySQL UPDATE query.
   */
  template  <typename ObjectType>
  class     MySQLUpdate : public Update<ObjectType>
  {
   private:
    sql::Connection* myconn_;
    sql::PreparedStatement* stmt_;

    /**
     *  MySQLUpdate copy constructor.
     */
            MySQLUpdate(const MySQLUpdate& myupdate)
      : Update<ObjectType>(myupdate.mapping_)
    {
      (void)myupdate;
    }

    /**
     *  MySQLUpdate operator= overload.
     */
    MySQLUpdate& operator=(const MySQLUpdate& myupdate)
    {
      (void)myupdate;
      return (*this);
    }

   public:
    /**
     *  MySQLUpdate default constructor.
     */
            MySQLUpdate(const Mapping<ObjectType>& mapping,
                        sql::Connection* myconn)
      : Update<ObjectType>(mapping), myconn_(myconn), stmt_(NULL) {}

    /**
     *  MySQLUpdate destructor.
     */
    virtual ~MySQLUpdate()
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
      decltype(this->mapping_.setters_.begin()) it;
      decltype(this->mapping_.setters_.end()) it_end;
      std::string query;

      query = "UPDATE ";
      query += this->mapping_.table_;
      query += " SET ";
      it_end = this->mapping_.setters_.end();
      for (it = this->mapping_.setters_.begin(); it != it_end; it++)
	{
	  query += (*it).first;
	  query += "=?, ";
	}
      query.resize(query.size() - 2);
      query += "WHERE ";
      it_end = this->uniques_.end();
      for (it = this->uniques_.begin(); it != it_end; it++)
	{
	  query += (*it).first;
	  query += "=? AND ";
	}
      query.resize(query.size() - 5);
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

#endif /* !MYSQL_UPDATE_HPP_ */
