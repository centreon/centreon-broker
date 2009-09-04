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

#ifndef DB_ORACLE_UPDATE_H_
# define DB_ORACLE_UPDATE_H_

# include <ctime>
# include <string>
# include "db/oracle/have_args.h"
# include "db/oracle/have_predicate.h"
# include "db/update.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class OracleUpdate update.h "db/oracle/update.h"
     *
     *  Oracle UPDATE query.
     *
     *  \see Update
     */
    class           OracleUpdate :  public OracleHaveArgs,
                                    public OracleHavePredicate,
                                    virtual public Update

    {
     private:
      std::list<std::string>::iterator
                    field_;
      void          CheckArg();
      void          GenerateQueryBeginning();

     protected:
                    OracleUpdate(const OracleUpdate& oupdate);
      OracleUpdate& operator=(const OracleUpdate& oupdate);
      unsigned int  GetArgCount() throw ();

     public:
                    OracleUpdate(OCI_Connection* oconn);
      virtual       ~OracleUpdate();
      void          Execute();
      unsigned int  GetUpdateCount();
      virtual void  Prepare();
      virtual void  SetArg(bool arg);
      virtual void  SetArg(double arg);
      virtual void  SetArg(int arg);
      virtual void  SetArg(short arg);
      virtual void  SetArg(const std::string& arg);
      virtual void  SetArg(time_t arg);
    };

    /**
     *  \class OracleMappedUpdate update.h "db/oracle/update.h"
     *
     *  Object-relational Oracle UPDATE query.
     *
     *  \see MappedUpdate
     */
    template              <typename T>
    class                 OracleMappedUpdate : public MappedUpdate<T>,
                                              public OracleUpdate
    {
     protected:
      /**
       *  \brief OracleMappedUpdate copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] omupdate Object to copy data from.
       */
                          OracleMappedUpdate(const OracleMappedUpdate& omu)
        : MappedUpdate<T>(omu), OracleUpdate(omu) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] omupdate Object to copy data from.
       *
       *  \return *this
       */
      OracleMappedUpdate& operator=(const OracleMappedUpdate& omupdate)
      {
	this->MappedUpdate<T>::operator=(omupdate);
	this->OracleUpdate::operator=(omupdate);
	return (*this);
      }

     public:
      /**
       *  \brief OracleMappedUpdate constructor.
       *
       *  Build the OracleMappedUpdate object. Needs the Oracle connection
       *  object on which the query will be executed.
       *
       *  \param[in] oconn   Oracle connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                          OracleMappedUpdate(OCI_Connection* oconn,
                                             const MappingGetters<T>& mapping)
        : MappedUpdate<T>(mapping), OracleUpdate(oconn) {}

      /**
       *  \brief OracleMappedUpdate destructor.
       *
       *  Release acquired ressources.
       */
                          ~OracleMappedUpdate() {}
    };
  }
}

#endif /* !DB_ORACLE_UPDATE_H_ */
