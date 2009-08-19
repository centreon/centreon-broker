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

#ifndef DB_MYSQL_UPDATE_H_
# define DB_MYSQL_UPDATE_H_

# include <ctime>
# include <string>
# include "db/mysql/have_args.h"
# include "db/mysql/have_predicate.h"
# include "db/update.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class MySQLUpdate update.h "db/mysql/update.h"
     *
     *  MySQL UPDATE query.
     *
     *  \see Update
     */
    class          MySQLUpdate :  public MySQLHaveArgs,
                                  public MySQLHavePredicate,
                                  virtual public Update

    {
     private:
      std::list<std::string>::iterator
                   field_;
      void         CheckArg();
      void         GenerateQueryBeginning();

     protected:
                   MySQLUpdate(const MySQLUpdate& myupdate);
      MySQLUpdate& operator=(const MySQLUpdate& myupdate);
      unsigned int GetArgCount() throw ();

     public:
                   MySQLUpdate(MYSQL* myconn);
      virtual      ~MySQLUpdate();
      void         Execute();
      unsigned int GetUpdateCount();
      virtual void Prepare();
      virtual void SetArg(bool arg);
      virtual void SetArg(double arg);
      virtual void SetArg(int arg);
      virtual void SetArg(short arg);
      virtual void SetArg(const std::string& arg);
      virtual void SetArg(time_t arg);
    };

    /**
     *  \class MySQLMappedUpdate update.h "db/mysql/update.h"
     *
     *  Object-relational MySQL UPDATE query.
     *
     *  \see MappedUpdate
     */
    template             <typename T>
    class                MySQLMappedUpdate : public MappedUpdate<T>,
                                             public MySQLUpdate
    {
     protected:
      /**
       *  \brief MySQLMappedUpdate copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] mymupdate Object to copy data from.
       */
                         MySQLMappedUpdate(const MySQLMappedUpdate& mymupdate)
        : MappedUpdate<T>(mymupdate), MySQLUpdate(mymupdate) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] mymupdate Object to copy data from.
       *
       *  \return *this
       */
      MySQLMappedUpdate& operator=(const MySQLMappedUpdate& mymupdate)
      {
	this->MappedUpdate<T>::operator=(mymupdate);
	this->MySQLUpdate::operator=(mymupdate);
	return (*this);
      }

     public:
      /**
       *  \brief MySQLMappedUpdate constructor.
       *
       *  Build the MySQLMappedUpdate object. Needs the MySQL connection object
       *  on which the query will be executed.
       *
       *  \param[in] myconn  MySQL connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                         MySQLMappedUpdate(MYSQL* myconn,
                                           const MappingGetters<T>& mapping)
        : MappedUpdate<T>(mapping), MySQLUpdate(myconn) {}

      /**
       *  \brief MySQLMappedUpdate destructor.
       *
       *  Release acquired ressources.
       */
                        ~MySQLMappedUpdate() {}
    };
  }
}

#endif /* !DB_MYSQL_UPDATE_H_ */
