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

#ifndef CONF_MANAGER_H_
# define CONF_MANAGER_H_

# include <list>
# include <map>
# include <string>
# include "conf/input.h"
# include "conf/log.h"
# include "conf/output.h"

namespace             CentreonBroker
{
  // Forward declarations
  class               ClientAcceptor;
  class               DBOutput;

  namespace           Conf
  {
    /**
     *  \class Manager manager.h "conf/manager.h"
     *  \brief Parse a file and configure internal state accordingly.
     *
     *  The ConfManager will handle parsing of CentreonBroker's configuration
     *  file and create/delete objects as necessary. User can request an update
     *  by sending SIGUSR1 to the process.
     *
     *  \see GetType
     */
    class             Manager
    {
     private:
      std::string     filename_;
      std::map<Input, CentreonBroker::ClientAcceptor*>
                      inputs_;
      std::list<Log>  logs_;
      std::map<Output, CentreonBroker::DBOutput*>
                      outputs_;
                      Manager();
                      Manager(const Manager& manager);
      Manager&        operator=(const Manager& manager);
      void            Parse(std::list<Input>& inputs,
                            std::list<Log>& logs,
                            std::list<Output>& outputs);

     public:
                      ~Manager();
      void            Close();
      static Manager& GetInstance();
      void            Open(const std::string& filename);
      void            Update();
    };
  }
}

#endif /* !CONF_MANAGER_H_ */
