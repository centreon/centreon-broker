/*
** query.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#ifndef DB_QUERY_H_
# define DB_QUERY_H_

# include <string>
# include <sys/types.h>
# include <vector>

namespace                    CentreonBroker
{
  class                      Query
  {
   private:
    void                     InternalCopy(const Query& query);

   protected:
    std::vector<std::string> fields_;
    std::string              table_;

   public:
                             Query();
                             Query(const Query& query);
    virtual                  ~Query();
    Query&                   operator=(const Query& query);
    void                     AddFields(const char **fields);
    virtual void             Execute() = 0;
    virtual void             Prepare() = 0;
    virtual void             SetDouble(int arg, double value) = 0;
    virtual void             SetInt(int arg, int value) = 0;
    virtual void             SetShort(int arg, short value) = 0;
    virtual void             SetString(int arg, const std::string& value) = 0;
    virtual void             SetTimeT(int arg, time_t value) = 0;
    void                     SetTable(const std::string& table);
  };
}

#endif /* !DB_QUERY_H_ */
