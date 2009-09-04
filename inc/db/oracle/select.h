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

#ifndef DB_ORACLE_SELECT_H_
# define DB_ORACLE_SELECT_H_

# include <string>
# include "db/oracle/have_args.h"
# include "db/oracle/have_predicate.h"
# include "db/select.h"

namespace            CentreonBroker
{
  namespace          DB
  {
    /**
     *  \class OracleSelect select.h "db/oracle/select.h"
     *
     *  Oracle SELECT query
     *
     *  \see Select
     */
    class            OracleSelect : virtual public Select,
                                    public OracleHaveArgs,
                                    public OracleHavePredicate
    {
     private:
      unsigned int   current_;
                     OracleSelect(const OracleSelect& os);
      OracleSelect&  operator=(const OracleSelect& os);
      void           GenerateQueryStart();
      unsigned int   GetArgCount() throw ();

     public:
                     OracleSelect(OCI_Connection* oconn);
      virtual        ~OracleSelect();
      virtual void   Execute();
      virtual bool   GetBool();
      virtual double GetDouble();
      virtual int    GetInt();
      virtual short  GetShort();
      virtual void   GetString(std::string& str);
      virtual bool   Next();
      virtual void   Prepare();
    };

    /**
     *  \class OracleMappedSelect select.h "db/oracle/select.h"
     *
     *  Object-relational Oracle SELECT query.
     *
     *  \see MappedSelect
     */
    template              <typename T>
    class                 OracleMappedSelect : public MappedSelect<T>,
                                               public OracleSelect
    {
     protected:
      /**
       *  \brief OracleMappedSelect copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] oms Object to copy data from.
       */
                          OracleMappedSelect(const OracleMappedSelect& oms)
        : MappedSelect<T>(oms), OracleSelect(oms) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] oms Object to copy data from.
       *
       *  \return *this
       */
      OracleMappedSelect& operator=(const OracleMappedSelect& oms)
      {
	this->MappedSelect<T>::operator=(oms);
	this->OracleSelect::operator=(oms);
	return (*this);
      }

     public:
      /**
       *  \brief OracleMappedSelect constructor.
       *
       *  Build the OracleMappedSelect object. Needs the Oracle connection
       *  object on which the query will be executed.
       *
       *  \param[in] oconn   Oracle connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                          OracleMappedSelect(OCI_Connection* oconn,
                                             const MappingSetters<T>& mapping)
	: MappedSelect<T>(mapping), OracleSelect(oconn) {}

      /**
       *  \brief OracleMappedInsert destructor.
       *
       *  Release acquired ressources.
       */
                          ~OracleMappedSelect() {}
    };
  }
}

#endif /* !DB_ORACLE_SELECT_H_ */
