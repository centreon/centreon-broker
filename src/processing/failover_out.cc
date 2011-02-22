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

#include <assert.h>
#include <stdlib.h>
#include "concurrency/lock.hh"
#include "config/factory.hh"
#include "config/interface.hh"
#include "interface/destination.hh"
#include "interface/source_destination.hh"
#include "logging/logging.hh"
#include "processing/failover_out.hh"

using namespace processing;

/**************************************
*                                     *
*             Local Class             *
*                                     *
**************************************/

namespace               processing {
  class                 failover_out_as_in : public failover_out_base {
    friend class        failover_out_base;

   private:
    interface::source*  _source;
    concurrency::mutex  _sourcem;
    std::auto_ptr<interface::source_destination>
                        _source_dest;
    concurrency::mutex  _source_destm;
    std::auto_ptr<config::interface>
                        _source_dest_conf;
                        failover_out_as_in(failover_out_as_in const& foai);
    failover_out_as_in& operator=(failover_out_as_in const& foai);

   public:
                        failover_out_as_in();
                        ~failover_out_as_in();
    void                close();
    void                connect();
    events::event*      event();
    void                exit();
    void                event(events::event* e);
    void                run(interface::source* source,
                          config::interface const& dest_conf,
                          concurrency::thread_listener* tl = NULL);
  };
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                            failover_out_base                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  failover_out_base is not copyable. Therefore any attempt to use the
 *  copy constructor will result in a call to abort().
 *
 *  @param[in] fob Unused.
 */
failover_out_base::failover_out_base(failover_out_base const& fob)
  : interface::source(),
    interface::destination(),
    feeder(),
    interface::source_destination() {
  (void)fob;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  failover_out_base is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] fob Unused.
 *
 *  @return This object.
 */
failover_out_base& failover_out_base::operator=(failover_out_base const& fob) {
  (void)fob;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
failover_out_base::failover_out_base() {}

/**
 *  Destructor.
 */
failover_out_base::~failover_out_base() {}

/**
 *  Failover thread entry point.
 */
void failover_out_base::operator()() {
  while (!_should_exit) {
    try {
      logging::debug << logging::HIGH << "connecting output";
      this->connect();
      logging::debug << logging::HIGH << "output connection successful";
      if (_failover.get()) {
        logging::debug << logging:: HIGH << "fetching events from the failover object";
        if (_failover->_source_dest.get())
          feed(_failover->_source_dest.get(), this);
        logging::debug << logging::HIGH << "killing failover thread";
        _failover->exit();
        _failover->join();
        logging::debug << logging::HIGH << "destroying failover object";
        _failover.reset();
      }
      logging::debug << logging::HIGH << "launching feeding loop";
      feed(this, this);
    }
    catch (std::exception const& e) {
      logging::error << logging::HIGH
                     << "error while processing output: " << e.what();
      if (!_failover.get() && _dest_conf->failover.get()) {
        logging::info << logging::HIGH << "launching output failover";
        _failover.reset(new failover_out_as_in);
        _failover->run(this, *_dest_conf->failover);
      }
      // XXX : configure sleeping time
      logging::info << logging::MEDIUM
                    << "waiting 5 seconds before attempting reconnection";
      sleep(5);
    }
    catch (...) {
      logging::error << logging::HIGH
                     << "output processing thread caught an unknown exception";
      logging::info << logging::INFO
                    << "output processing thread will now exit";
      break ;
    }
  }
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                FailoverOut                                  *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  failover_out is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] fo Unused.
 */
failover_out::failover_out(failover_out const& fo)
  : interface::source(), interface::destination(), failover_out_base() {
  (void)fo;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  failover_out is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] fo Unused.
 *
 *  @return This object.
 */
failover_out& failover_out::operator=(failover_out const& fo) {
  (void)fo;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
failover_out::failover_out() {}

/**
 *  Destructor.
 */
failover_out::~failover_out() {
  this->exit();
  join();
}

/**
 *  Close current objects.
 */
void failover_out::close() {
  {
    concurrency::lock l(_sourcem);
    if (_source.get())
      _source->close();
  }
  {
    concurrency::lock l(_destm);
    if (_dest.get())
      _dest->close();
  }
  return ;
}

/**
 *  Connect the destination object.
 */
void failover_out::connect() {
  // Delete destination first before opening another.
  concurrency::lock l(_destm);
  _dest.reset();
  _dest.reset(config::factory::build_destination(*_dest_conf));
  return ;
}

/**
 *  Get the next available event from the source object.
 *
 *  @return Next available event from the source object.
 */
events::event* failover_out::event() {
  concurrency::lock l(_sourcem);
  return (_source->event());
}

/**
 *  Store event in the destination object.
 *
 *  @param[in] e Event to store.
 */
void failover_out::event(events::event* e) {
  concurrency::lock l(_destm);
  _dest->event(e);
  return ;
}

/**
 *  Tell the processing thread to stop ASAP.
 */
void failover_out::exit() {
  {
    if (_source.get())
      _source->close();
  }
  feeder::exit();
  return ;
}

/**
 *  Launch failover thread on output object.
 *
 *  @param[in] source    Source object.
 *  @param[in] dest_conf Configuration of the destination object.
 *  @param[in] tl        Listener of the thread.
 */
void failover_out::run(interface::source* source,
                       config::interface const& dest_conf,
                       concurrency::thread_listener* tl) {
  {
    concurrency::lock l(_destm);
    _dest_conf.reset(new config::interface(dest_conf));
  }
  {
    concurrency::lock l(_sourcem);
    _source.reset(source);
  }
  concurrency::thread::run(tl);
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                             FailoverOutAsIn                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  failover_out_as_in it not copyable, therefore any attempt to use the
 *  copy constructor will result in a call to abort().
 *
 *  @param[in] foai Unused.
 */
failover_out_as_in::failover_out_as_in(failover_out_as_in const& foai)
  : interface::source(), interface::destination(), failover_out_base() {
  (void)foai;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  failover_out_as_in is not copyable, therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] foai Unused.
 *
 *  @return This object.
 */
failover_out_as_in& failover_out_as_in::operator=(failover_out_as_in const& foai) {
  (void)foai;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
failover_out_as_in::failover_out_as_in() {}

/**
 *  Destructor.
 */
failover_out_as_in::~failover_out_as_in() {
  try {
    this->exit();
    join();
  }
  catch (...) {}
}

/**
 *  Close failover_out_as_in.
 */
void failover_out_as_in::close() {
  concurrency::lock l(_source_destm);
  _source_dest.reset();
  return ;
}

/**
 *  Connect output object.
 */
void failover_out_as_in::connect() {
  // Close before reopening.
  concurrency::lock l(_source_destm);
  _source_dest.reset();
  _source_dest.reset(config::factory::build_sourcedestination(
    *_source_dest_conf));
  return ;
}

/**
 *  Get next available event.
 */
events::event* failover_out_as_in::event() {
  concurrency::lock l(_sourcem);
  return (_source->event());
}

/**
 *  Store event.
 *
 *  @param[in] e Event to store.
 */
void failover_out_as_in::event(events::event* e) {
  concurrency::lock l(_source_destm);
  static_cast<interface::destination*>(_source_dest.get())->event(e);
  return ;
}

/**
 *  Ask the thread to exit ASAP.
 */
void failover_out_as_in::exit() {
  this->close();
  feeder::exit();
  return ;
}

/**
 *  Launch failover thread.
 */
void failover_out_as_in::run(interface::source* source,
                             config::interface const& dest_conf,
                             concurrency::thread_listener* tl) {
  {
    concurrency::lock l(_source_destm);
    _source_dest_conf.reset(new config::interface(dest_conf));
  }
  {
    concurrency::lock l(_sourcem);
    _source = source;
  }
  concurrency::thread::run(tl);
  return ;
}
