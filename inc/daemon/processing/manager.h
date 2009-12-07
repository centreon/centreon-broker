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

#ifndef PROCESSING_MANAGER_H_
# define PROCESSING_MANAGER_H_

# include <list>
# include "concurrency/mutex.h"
# include "concurrency/thread_listener.h"

namespace              Processing
{
  /**
   *  \class Manager manager.h "processing/manager.h"
   *  \brief Manage event source objects.
   *
   *  The Processing::Manager class handles objects that generates or store
   *  events. These objects are registered and deleted through this singleton.
   */
  class                Manager : public Concurrency::ThreadListener
  {
   private:
    std::list<Concurrency::Thread*>
                       threads_;
    Concurrency::Mutex threadsm_;
                       Manager();
                       Manager(const Manager& manager);
                       ~Manager();
    Manager&           operator=(const Manager& manager);

   public:
    void               Delete(const Concurrency::Thread* thread);
    static Manager&    Instance();
    void               Manage(Concurrency::Thread* thread);
    void               OnCreate(Concurrency::Thread* thread);
    void               OnExit(Concurrency::Thread* thread);
  };
}

#endif /* !PROCESSING_MANAGER_H_ */
