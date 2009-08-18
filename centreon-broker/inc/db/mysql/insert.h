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

#ifndef DB_MYSQL_INSERT_H_
# define DB_MYSQL_INSERT_H_

# include <ctime>
# include <string>
# include "db/insert.h"
# include "db/mysql/have_args.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class MySQLInsert insert.h "db/mysql/insert.h"
     *
     *  MySQL INSERT query.
     *
     *  \see Insert
     */
    class          MySQLInsert : virtual public Insert, public MySQLHaveArgs
    {
     private:
      void         GenerateQueryBeginning();
      unsigned int GetArgCount() throw ();

     protected:
                   MySQLInsert(const MySQLInsert& myinsert);
      MySQLInsert& operator=(const MySQLInsert& myinsert);

     public:
                   MySQLInsert(MYSQL* myconn);
      virtual      ~MySQLInsert();
      void         Execute();
      void         Prepare();
      virtual void SetArg(bool arg);
      virtual void SetArg(double arg);
      virtual void SetArg(int arg);
      virtual void SetArg(short arg);
      virtual void SetArg(const std::string& arg);
      virtual void SetArg(time_t arg);
    };

    /**
     *  \class MySQLMappedInsert insert.h "db/mysql/insert.h"
     *
     *  Object-relational MySQL INSERT query.
     *
     *  \see MappedInsert
     */
    template       <typename T>
    class          MySQLMappedInsert : public MappedInsert<T>,
                                       public MySQLInsert
    {
     protected:
      /**
       *  \brief MySQLMappedInsert copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] myminsert Object to copy data from.
       */
                         MySQLMappedInsert(const MySQLMappedInsert& myminsert)
	: MappedInsert<T>(myminsert), MySQLInsert(myminsert) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] myminsert Object to copy data from.
       *
       *  \return *this
       */
      MySQLMappedInsert& operator=(const MySQLMappedInsert& myminsert)
      {
	this->MappedInsert<T>::operator=(myminsert);
	this->MySQLInsert::operator=(myminsert);
	return (*this);
      }

     public:
      /**
       *  \brief MySQLMappedInsert constructor.
       *
       *  Build the MySQLMappedInsert object. Needs the MySQL connection object
       *  on which the query will be executed.
       *
       *  \param[in] myconn  MySQL connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                         MySQLMappedInsert(MYSQL* myconn,
                                           const Mapping<T>& mapping)
        : MappedInsert<T>(mapping), MySQLInsert(myconn) {}

      /**
       *  \brief MySQLMappedInsert destructor.
       *
       *  Release acquired ressources.
       */
                         ~MySQLMappedInsert() {}

      /**
       *  \brief Prepare the query.
       *
       *  Prepare the INSERT query on the MySQL server.
       */
      void               Prepare()
      {
	this->MappedInsert<T>::ExtractGetters();
	this->MySQLInsert::Prepare();
	return ;
      }
    };
  }
}

#endif /* !DB_MYSQL_INSERT_H_ */
