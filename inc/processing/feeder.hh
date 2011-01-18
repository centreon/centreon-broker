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

#ifndef PROCESSING_FEEDER_HH_
# define PROCESSING_FEEDER_HH_

# include <memory>
# include "concurrency/mutex.hh"
# include "concurrency/thread.hh"
# include "interface/source_destination.hh"

namespace              processing {
  /**
   *  @class feeder feeder.hh "processing/feeder.hh"
   *  @brief Get events from a source and bring it to a destination.
   *
   *  The feeder class implements the mediator pattern and avoid event
   *  sources to be tightly coupled with the event publisher.
   *
   *  @see events::event
   *  @see multiplexing::publisher
   */
  class                feeder : public concurrency::thread {
   private:
                       feeder(feeder const& f);
    feeder&            operator=(feeder const& f);

   public:
                       feeder();
    virtual            ~feeder();
    void               feed(interface::source* source,
                         interface::destination* dest);
  };

  /**
   *  @class feeder_once feeder.hh "processing/feeder.hh"
   *  @brief Get events from a source and bring it to a destination with
   *         no failover.
   *
   *  Act on two already opened interface and send events from the
   *  source to the destination. If an error occur both interfaces are
   *  destroyed.
   */
  class                feeder_once : public feeder,
                                     public interface::source_destination {
   private:
    std::auto_ptr<interface::destination>
                       _dest;
    concurrency::mutex _destm;
    std::auto_ptr<interface::source>
                       _source;
    concurrency::mutex _sourcem;
                       feeder_once(feeder_once const& fo);
    feeder_once&       operator=(feeder_once const& fo);

   public:
                       feeder_once();
    virtual            ~feeder_once();
    virtual void       operator()();
    void               close();
    events::event*     event();
    void               event(events::event* e);
    void               exit();
    void               run(interface::source* source,
                         interface::destination* dest,
                         concurrency::thread_listener* tl = NULL);
  };
}

#endif /* !PROCESSING_FEEDER_HH_ */
