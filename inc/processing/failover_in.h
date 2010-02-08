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

#ifndef PROCESSING_FAILOVER_IN_H_
# define PROCESSING_FAILOVER_IN_H_

# include <memory>
# include "concurrency/mutex.h"
# include "interface/sourcedestination.h"
# include "processing/feeder.h"

// Forward declaration.
namespace              Configuration
{ class                Interface; }

namespace              Processing
{
  class                FailoverIn : public Feeder,
                                    public Interface::SourceDestination
  {
   private:
    std::auto_ptr<Interface::Destination>
                       dest_;
    Concurrency::Mutex destm_;
    std::auto_ptr<Interface::Source>
                       source_;
    Concurrency::Mutex sourcem_;
    std::auto_ptr<Configuration::Interface>
                       source_conf_;
                       FailoverIn(const FailoverIn& fi);
    FailoverIn&        operator=(const FailoverIn& fi);

   public:
                       FailoverIn();
                       ~FailoverIn();
    void               operator()();
    void               Close();
    void               Connect();
    Events::Event*     Event();
    void               Event(Events::Event* event);
    void               Exit();
    void               Run(const Configuration::Interface& source_conf,
                           Interface::Destination* destination,
                           Concurrency::ThreadListener* tl = NULL);
  };
}

#endif /* !PROCESSING_FAILOVER_IN_H_ */
