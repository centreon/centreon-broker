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

#ifndef DB_MYSQL_SELECT_H_
# define DB_MYSQL_SELECT_H_

# include <mysql.h>
# include <string>
# include "db/mysql/have_args.h"
# include "db/mysql/have_predicate.h"
# include "db/select.h"

namespace              CentreonBroker
{
  namespace            DB
  {
    /**
     *  \class MySQLSelect select.h "db/mysql/select.h"
     *
     *  MySQL SELECT query
     *
     *  \see Select
     */
    class              MySQLSelect : virtual public Select,
                                     public MySQLHaveArgs,
                                     public MySQLHavePredicate
    {
     private:
      unsigned int     current_;
      union
      {
	struct
	{
	  MYSQL_RES*   res;
	  MYSQL_ROW    row;
	}              std;
	struct
	{
	  unsigned int count;
	  MYSQL_BIND*  res;
	}              stmt;
      }                result_;
                       MySQLSelect(const MySQLSelect& mys);
      MySQLSelect&     operator=(const MySQLSelect& mys);
      void             CleanArgs();
      void             GenerateQueryStart();
      unsigned int     GetArgCount() throw ();

     public:
                       MySQLSelect(MYSQL* mysql);
      virtual          ~MySQLSelect();
      virtual void     Execute();
      virtual bool     GetBool();
      virtual double   GetDouble();
      virtual int      GetInt();
      virtual short    GetShort();
      virtual void     GetString(std::string& str);
      virtual bool     Next();
      virtual void     Prepare();
    };

    /**
     *  \class MySQLMappedSelect select.h "db/mysql/select.h"
     *
     *  Object-relational MySQL SELECT query.
     *
     *  \see MappedSelect
     */
    template             <typename T>
    class                MySQLMappedSelect : public MappedSelect<T>,
                                             public MySQLSelect
    {
     protected:
      /**
       *  \brief MySQLMappedSelect copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] myms Object to copy data from.
       */
                         MySQLMappedSelect(const MySQLMappedSelect& myms)
        : MappedSelect<T>(myms), MySQLSelect(myms) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] myms Object to copy data from.
       *
       *  \return *this
       */
      MySQLMappedSelect& operator=(const MySQLMappedSelect& myms)
      {
	this->MappedSelect<T>::operator=(myms);
	this->MySQLSelect::operator=(myms);
	return (*this);
      }

     public:
      /**
       *  \brief MySQLMappedSelect constructor.
       *
       *  Build the MySQLMappedSelect object. Needs the MySQL connection object
       *  on which the query will be executed.
       *
       *  \param[in] myconn  MySQL connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                         MySQLMappedSelect(MYSQL* myconn,
                                           const MappingSetters<T>& mapping)
	: MappedSelect<T>(mapping), MySQLSelect(myconn) {}

      /**
       *  \brief MySQLMappedInsert destructor.
       *
       *  Release acquired ressources.
       */
                         ~MySQLMappedSelect() {}
    };
  }
}

#endif /* !DB_MYSQL_SELECT_H_ */
