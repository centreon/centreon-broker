/*
** truncate.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_TRUNCATE_H_
# define DB_TRUNCATE_H_

# include "db/db_exception.h"
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  Represents a table truncation.
     */
    class          Truncate : public Query
    {
     private:
                   Truncate(const Truncate& truncate) throw ();
      Truncate&    operator=(const Truncate& truncate) throw ();

     public:
                   Truncate() throw ();
      virtual      ~Truncate();
      virtual void Execute() throw (DBException) = 0;
    };
  }
}

#endif /* !DB_TRUNCATE_H_ */
