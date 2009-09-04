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

#ifndef DB_ORACLE_TRUNCATE_H_
# define DB_ORACLE_TRUNCATE_H_

# include "db/oracle/query.h"
# include "db/truncate.h"

namespace            CentreonBroker
{
  namespace          DB
  {
    /**
     *  \class OracleTruncate truncate.h "db/oracle/truncate.h"
     *
     *  Oracle TRUNCATE query.
     *
     *  \see Truncate
     */
    class             OracleTruncate : public Truncate, public OracleQuery
    {
     private:
                      OracleTruncate(const OracleTruncate& otruncate);
      OracleTruncate& operator=(const OracleTruncate& otruncate);
      void            BuildQuery();

     public:
                      OracleTruncate(OCI_Connection* oconn);
                      ~OracleTruncate();
      void            Execute();
      void            Prepare();
    };
  }
}

#endif /* !DB_ORACLE_TRUNCATE_H_ */
