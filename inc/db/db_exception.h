/*
** db_exception.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/08/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_EXCEPTION_H_
# define DB_EXCEPTION_H_

# include "exception.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          DBException : public CentreonBroker::Exception
    {
     public:
      enum         Reason
      {
	UNKNOWN = 0,
	COMMIT,
	INITIALIZATION,
	QUERY_EXECUTION,
	QUERY_PREPARATION
      };

     private:
      Reason       reason_;
      void         InternalCopy(const DBException& dbe) throw ();

     public:
                   DBException(const DBException& dbe);
		   DBException(int val,
                               Reason reason = UNKNOWN,
                               const char* msg = NULL);
                   ~DBException() throw ();
      DBException& operator=(const DBException& dbe);
    };
  }
}

#endif /* !DB_EXCEPTION_H_ */
