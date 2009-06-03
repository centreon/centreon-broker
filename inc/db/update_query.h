/*
** update_query.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#ifndef DB_UPDATE_QUERY_H_
# define DB_UPDATE_QUERY_H_

# include <string>
# include <vector>
# include "db/query.h"

namespace                    CentreonBroker
{
  class                      UpdateQuery : public Query
  {
   private:
    void                     InternalCopy(const UpdateQuery& update_query);

   protected:
    std::vector<std::string> uniques_;

   public:
                             UpdateQuery();
                             UpdateQuery(const UpdateQuery& update_query);
    virtual                  ~UpdateQuery();
    UpdateQuery&             operator=(const UpdateQuery& update_query);
    void                     AddUniques(const char** uniques);
  };
}

#endif /* !DB_UPDATE_QUERY_H_ */
