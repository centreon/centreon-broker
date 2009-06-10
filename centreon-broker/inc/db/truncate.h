/*
** truncate.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#ifndef DB_TRUNCATE_H_
# define DB_TRUNCATE_H_

# include <string>
# include "db/query.h"

namespace                CentreonBroker
{
  namespace              DB
  {
    /**
     *  Represents a table truncation.
     */
    class                Truncate : virtual public Query
    {
     private:
                         Truncate(const Truncate& truncate) throw ();
      Truncate&          operator=(const Truncate& truncate) throw ();

     protected:
      std::string        table_;

     public:
                         Truncate() throw ();
      virtual            ~Truncate();
      virtual void       Execute() = 0;
      const std::string& GetTable() const throw ();
      void               SetTable(const std::string& table);
    };
  }
}

#endif /* !DB_TRUNCATE_H_ */
