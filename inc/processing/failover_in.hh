/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#ifndef PROCESSING_FAILOVER_IN_HH_
# define PROCESSING_FAILOVER_IN_HH_

# include <memory>
# include "concurrency/mutex.hh"
# include "interface/source_destination.hh"
# include "processing/feeder.hh"

// Forward declaration.
namespace              config
{ class                interface; }

namespace              processing {
  class                failover_in : public feeder,
                                     public interface::source_destination {
   private:
    std::auto_ptr<interface::destination>
                       _dest;
    concurrency::mutex _destm;
    std::auto_ptr<interface::source>
                       _source;
    concurrency::mutex _sourcem;
    std::auto_ptr<config::interface>
                       _source_conf;
                       failover_in(failover_in const& fi);
    failover_in&       operator=(failover_in const& fi);

   public:
                       failover_in();
                       ~failover_in();
    void               operator()();
    void               close();
    void               connect();
    events::event*     event();
    void               event(events::event* e);
    void               exit();
    void               run(config::interface const& source_conf,
                         interface::destination* dest,
                         concurrency::thread_listener* tl = NULL);
  };
}

#endif /* !PROCESSING_FAILOVER_IN_HH_ */
