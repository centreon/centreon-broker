/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef FILE_OUTPUT_H_
# define FILE_OUTPUT_H_

# include <fstream>
# include <string>
# include "condition_variable.h"
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

#endif /* !FILE_OUTPUT_H_ */
