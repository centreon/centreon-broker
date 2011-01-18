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

#ifndef PROCESSING_FAILOVER_OUT_HH_
# define PROCESSING_FAILOVER_OUT_HH_

# include <memory>
# include "concurrency/mutex.hh"
# include "interface/source_destination.hh"
# include "processing/feeder.hh"

// Forward declarations.
namespace              config
{ class                interface; }

namespace              processing {
  // Forward declaration.
  class                failover_out_as_in;

  /**
   *  @class failover_out_base failover_out.hh "processing/failover_out.hh"
   *  @brief Base class of failover_out and failover_out_as_in.
   *
   *  failover_out_base holds common components between objects in
   *  charge of handling failover of outputs.
   */
  class                failover_out_base : public feeder,
                                           public interface::source_destination {
   private:
                       failover_out_base(failover_out_base const& fob);
    failover_out_base& operator=(failover_out_base const& fob);

   protected:
    std::auto_ptr<config::interface>
                       _dest_conf;
    std::auto_ptr<failover_out_as_in>
                       _failover;

   public:
                       failover_out_base();
    virtual            ~failover_out_base();
    void               operator()();
    virtual void       connect() = 0;
  };

  class                failover_out : public failover_out_base {
   private:
    std::auto_ptr<interface::destination>
                       _dest;
    concurrency::mutex _destm;
    std::auto_ptr<interface::source>
                       _source;
    concurrency::mutex _sourcem;
                       failover_out(failover_out const& fo);
    failover_out&      operator=(failover_out const& fo);

   public:
                       failover_out();
                       ~failover_out();
    void               close();
    void               connect();
    events::event*     event();
    void               event(events::event* e);
    void               exit();
    void               run(interface::source* source,
                         config::interface const& dest_conf,
                         concurrency::thread_listener* tl = NULL);
  };
}

#endif /* !PROCESSING_FAILOVER_OUT_HH_ */
