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

#ifndef CONFIGURATION_MANAGER_H_
# define CONFIGURATION_MANAGER_H_

# include <list>
# include <map>
# include <string>
# include "concurrency/mutex.h"
# include "concurrency/thread_listener.h"
# include "configuration/interface.h"
# include "configuration/log.h"
# include "logging/logging.hh"

// Forward declarations
namespace              Concurrency
{ class                Thread; }

namespace              Configuration
{
  /**
   *  \class Manager manager.h "configuration/manager.h"
   *  \brief Parse a file and configure internal state accordingly.
   *
   *  The Manager will handle parsing of CentreonBroker's configuration file
   *  and create/delete objects as necessary. User can request an update by
   *  sending SIGHUP to the process.
   */
  class                Manager : public Concurrency::ThreadListener
  {
   private:
    std::string        filename_;
    std::map<Interface, Concurrency::Thread*>
                       inputs_;
    std::map<Log, logging::backend*>
                       logs_;
    Concurrency::Mutex mutex_;
    std::map<Interface, Concurrency::Thread*>
                       outputs_;
    std::list<Concurrency::Thread*>
                       spontaneous_;
    std::list<Concurrency::Thread*>
                       to_reap_;
                       Manager();
                       Manager(const Manager& manager);
    Manager&           operator=(const Manager& manager);
                       ~Manager();

   public:
    void               Close();
    static Manager&    Instance();
    void               OnCreate(Concurrency::Thread* thread);
    void               OnExit(Concurrency::Thread* thread);
    void               Open(const std::string& filename);
    void               Reap();
    void               Update();
  };
}

#endif /* !CONFIGURATION_MANAGER_H_ */
