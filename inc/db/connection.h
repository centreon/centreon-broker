/*
** connection.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 05/29/09 Matthieu Kermagoret
*/

#ifndef DB_CONNECTION_H_
# define DB_CONNECTION_H_

namespace                            CentreonBroker
{
  /**
   *  This class represents a connection to a DB server.
   */
  class                              DBConnection
  {
   private:
                                     DBConnection(const DBConnection& dbconn);
    DBConnection&                    operator=(const DBConnection& dbconn);

   public:
                                     DBConnection();
    virtual                          ~DBConnection();
  };
};

#endif /* !DB_CONNECTION_H_ */
