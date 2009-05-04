/*
** mysqloutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#ifndef MYSQLOUTPUT_H_
# define MYSQLOUTPUT_H_

# include "output.h"
# include "thread.h"

namespace		CentreonBroker
{
  class			MySQLOutput
    : public CentreonBroker::Output,
    public CentreonBroker::Thread
    {
    private:
			MySQLOutput(const MySQLOutput& mysqlo);
      MySQLOutput&	operator=(const MySQLOutput& mysqlo);

    public:
			MySQLOutput();
			~MySQLOutput();

      // Output
      void		Event(const CentreonBroker::Event& event);

      // Thread
      int		Core();
    };
}

#endif /* !MYSQLOUTPUT_H_ */
