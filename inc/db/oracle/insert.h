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

#ifndef DB_ORACLE_INSERT_H_
# define DB_ORACLE_INSERT_H_

# include <ctime>
# include <string>
# include "db/insert.h"
# include "db/oracle/have_args.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class OracleInsert insert.h "db/oracle/insert.h"
     *
     *  Oracle INSERT query.
     *
     *  \see Insert
     */
    class           OracleInsert : virtual public Insert, public OracleHaveArgs
    {
     private:
      void          GenerateQueryBeginning();
      unsigned int  GetArgCount() throw ();

     protected:
                    OracleInsert(const OracleInsert& oinsert);
      OracleInsert& operator=(const OracleInsert& oinsert);

     public:
                    OracleInsert(OCI_Connection* oconn);
      virtual       ~OracleInsert();
      void          Execute();
      unsigned int  InsertId();
      void          Prepare();
      virtual void  SetArg(bool arg);
      virtual void  SetArg(double arg);
      virtual void  SetArg(int arg);
      virtual void  SetArg(short arg);
      virtual void  SetArg(const std::string& arg);
      virtual void  SetArg(time_t arg);
    };

    /**
     *  \class OracleMappedInsert insert.h "db/oracle/insert.h"
     *
     *  Object-relational Oracle INSERT query.
     *
     *  \see MappedInsert
     */
    template              <typename T>
    class                 OracleMappedInsert : public MappedInsert<T>,
                                               public OracleInsert
    {
     protected:
      /**
       *  \brief OracleMappedInsert copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] omi Object to copy data from.
       */
                          OracleMappedInsert(const OracleMappedInsert& omi)
	: MappedInsert<T>(omi), OracleInsert(omi) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] ominsert Object to copy data from.
       *
       *  \return *this
       */
      OracleMappedInsert& operator=(const OracleMappedInsert& ominsert)
      {
	this->MappedInsert<T>::operator=(ominsert);
	this->OracleInsert::operator=(ominsert);
	return (*this);
      }

     public:
      /**
       *  \brief OracleMappedInsert constructor.
       *
       *  Build the OracleMappedInsert object. Needs the Oracle connection
       *  object on which the query will be executed.
       *
       *  \param[in] ocon n  Oracle connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                          OracleMappedInsert(OCI_Connection* oconn,
                                             const MappingGetters<T>& mapping)
        : MappedInsert<T>(mapping), OracleInsert(oconn) {}

      /**
       *  \brief OracleMappedInsert destructor.
       *
       *  Release acquired ressources.
       */
                          ~OracleMappedInsert() {}

      /**
       *  \brief Prepare the query.
       *
       *  Prepare the INSERT query on the Oracle server.
       */
      void                Prepare()
      {
	this->MappedInsert<T>::ExtractGetters();
	this->OracleInsert::Prepare();
	return ;
      }
    };
  }
}

#endif /* !DB_ORACLE_INSERT_H_ */
