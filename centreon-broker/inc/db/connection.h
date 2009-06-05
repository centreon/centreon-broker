/*
** connection.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#ifndef DB_CONNECTION_H_
# define DB_CONNECTION_H_

# include <string>

namespace                  CentreonBroker
{
  class                    Query;
  class                    TruncateQuery;
  class                    UpdateQuery;

  /**
   *  This class represents a connection to a DB server.
   */
  class                    DBConnection
  {
   public:
                           DBConnection();
                           DBConnection(const DBConnection& dbconn);
    virtual                ~DBConnection();
    DBConnection&          operator=(const DBConnection& dbconn);
    virtual void           Commit() = 0;
    virtual void           Connect(const std::string& host,
                                   const std::string& user,
                                   const std::string& password,
                                   const std::string& db) = 0;
    virtual void           Disconnect() = 0;
    virtual Query*         GetInsertQuery() = 0;
    virtual TruncateQuery* GetTruncateQuery() = 0;
    virtual UpdateQuery*   GetUpdateQuery() = 0;
  };
}

#endif /* !DB_CONNECTION_H_ */
