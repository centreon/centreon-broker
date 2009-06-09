/*
** query.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_QUERY_H_
# define DB_QUERY_H_

# include "db/db_exception.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    template       <typename ObjectType>
    class          Mapping;

    /**
     *  This is the root class of all queries. Every query can be prepared
     *  using the appropriate feature of the DBMS.
     */
    class          Query
    {
     private:
                   Query(const Query& query) throw ();
      Query&       operator=(const Query& query) throw ();

     public:
                   Query() throw ();
      virtual      ~Query();
      virtual void Prepare() throw (DBException) = 0;
    };
  }
}

#endif /* !DB_QUERY_H_ */
