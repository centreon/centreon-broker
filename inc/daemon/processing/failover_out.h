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

#ifndef PROCESSING_FAILOVER_OUT_H_
# define PROCESSING_FAILOVER_OUT_H_

# include <memory>
# include "concurrency/mutex.h"
# include "interface/sourcedestination.h"
# include "processing/feeder.h"

// Forward declarations.
namespace              Configuration
{ class                Interface; }

namespace              Processing
{
  // Forward declaration.
  class                FailoverOutAsIn;

  /**
   *  \class FailoverOutBase failover_out.h "processing/failover_out.h"
   *  \brief Base class of FailoverOut and FailoverOutAsIn.
   *
   *  FailoverOutBase holds common components between objects in charge of
   *  handling failover of outputs.
   */
  class                FailoverOutBase : public Feeder,
                                         public Interface::SourceDestination
  {
   private:
                       FailoverOutBase(const FailoverOutBase& fob);
    FailoverOutBase&   operator=(const FailoverOutBase& fob);

   protected:
    std::auto_ptr<Configuration::Interface>
                       dest_conf_;
    std::auto_ptr<FailoverOutAsIn>
                       failover_;

   public:
                       FailoverOutBase();
    virtual            ~FailoverOutBase();
    void               operator()();
    virtual void       Connect() = 0;
  };

  class                FailoverOut : public FailoverOutBase
  {
   private:
    std::auto_ptr<Interface::Destination>
                       dest_;
    Concurrency::Mutex destm_;
    std::auto_ptr<Interface::Source>
                       source_;
    Concurrency::Mutex sourcem_;
                       FailoverOut(const FailoverOut& fo);
    FailoverOut&       operator=(const FailoverOut& fo);

   public:
                       FailoverOut();
                       ~FailoverOut();
    void               Close();
    void               Connect();
    Events::Event*     Event();
    void               Event(Events::Event* event);
    void               Exit();
    void               Run(Interface::Source* source,
                           const Configuration::Interface& dest_conf,
                           Concurrency::ThreadListener* tl = NULL);
  };
}

#endif /* !PROCESSING_FAILOVER_OUT_H_ */
