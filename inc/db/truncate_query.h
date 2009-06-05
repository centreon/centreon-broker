/*
** truncate_query.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#ifndef TRUNCATE_QUERY_H_
# define TRUNCATE_QUERY_H_

# include <string>

namespace          CentreonBroker
{
  class            TruncateQuery
  {
   private:
    void           InternalCopy(const TruncateQuery& truncate_query);

   protected:
    std::string    table_;

   public:
                   TruncateQuery();
                   TruncateQuery(const TruncateQuery& truncate_query);
    virtual        ~TruncateQuery();
    TruncateQuery& operator=(const TruncateQuery& truncate_query);
    virtual void   Execute() = 0;
    void           SetTable(const std::string& table);
  };
}

#endif /* !TRUNCATE_QUERY_H_ */
