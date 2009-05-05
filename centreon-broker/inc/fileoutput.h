/*
** fileoutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#ifndef FILEOUTPUT_H_
# define FILEOUTPUT_H_

# include <fstream>
# include <string>
# include "conditionvariable.h"
# include "exception.h"
# include "mutex.h"
# include "thread.h"

namespace                             CentreonBroker
{
  class                               FileOutput
    : private CentreonBroker::Thread
  {
   private:
    std::string                       buffer;
    CentreonBroker::ConditionVariable condvar;
    volatile bool                     exit_thread;
    CentreonBroker::Mutex             mutex;
    std::ofstream                     ofs;
                                      FileOutput(const FileOutput& fileo);
    FileOutput&                       operator=(const FileOutput& fileo);

    // Thread method
    int                               Core();

   public:
                                      FileOutput();
                                      ~FileOutput();
    void                              Close();
    void                              Open(const std::string& filename)
      throw (CentreonBroker::Exception);

    // XXX : handle events in a thread-safe way
  };
}

#endif /* !FILEOUTPUT_H_ */
