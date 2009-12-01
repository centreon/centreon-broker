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
# include <string>
# include <utility>
# include "concurrency/mutex.h"

namespace              Processing
{
  // Forward declarations.
  class                Feeder;
  class                Listener;

  /**
   *  \class Manager manager.h "processing/manager.h"
   *  \brief Manage event source objects.
   *
   *  The Processing::Manager class handles objects that generates or store
   *  events. These objects are created/updated/deleted by this singleton
   *  according to the specified configuration.
   */
  class                Manager
  {
   private:
    std::list<std::pair<std::string, Feeder*> >
                       feeders_;
    std::list<std::pair<std::string, Listener*> >
                       listeners_;
    Concurrency::Mutex mutex_;
                       Manager();
                       Manager(const Manager& manager);
                       ~Manager();
    Manager&           operator=(const Manager& manager);

   public:
    void               Delete(const std::string& name);
    void               Delete(const Feeder* feeder);
    void               Delete(const Listener* listener);
    static Manager&    Instance();
    void               Manage(Feeder* feeder, const std::string& name = "");
    void               Manage(Listener* listener, const std::string& nam = "");
  };
}

#endif /* !PROCESSING_MANAGER_H_ */
