/*
** Copyright 2015 Merethis
**
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
*/

#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/downtime_removed.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/logging/logging.hh"

extern unsigned int instance_id;

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Constructor.
 *
 *  @param[in]      correlation_file  Correlation file.
 *  @param[int,out] cache             Persistent cache.
 *  @param[in]      passive           Is this stream passive ?
 *                                    (won't send any event)
 */
stream::stream(
          QString const& correlation_file,
    misc::shared_ptr<persistent_cache> cache,
    bool load_correlation,
    bool passive)
  : _correlation_file(correlation_file),
    _process_out(true),
    _cache(cache),
    _passive(passive) {
  // Create the engine started event.
  multiplexing::publisher pblsh;
  misc::shared_ptr<engine_state> es(new engine_state);
  es->instance_id = instance_id;
  es->started = true;
  pblsh.write(es);

  if (!_passive)
    _pblsh.reset(new multiplexing::publisher);

  // Load the correlation.
  if (load_correlation)
    _load_correlation();
}

/**
 *  Destructor.
 */
stream::~stream() {
  try {
    multiplexing::publisher pblsh;
    misc::shared_ptr<engine_state> es(new engine_state);
    es->instance_id = instance_id;
    pblsh.write(es);
  } catch (std::exception const& e) {
    logging::error(logging::medium)
      << "correlator: error while trying to publish engine state: "
      << e.what();
  }
}

/**
 *  Set which data to process.
 *
 *  @param[in] in   Process in.
 *  @param[in] out  Process out.
 */
void stream::process(bool in, bool out) {
  bool was_processing(_process_out);
  _process_out = in || !out; // Only for immediate shutdown.
  if (was_processing && !_process_out)
    _save_persistent_cache();
  return ;
}

/**
 *  Read data from the stream.
 *
 *  @param[out] d  Unused.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
	 << "correlation: cannot read from a stream. This is likely a "
	 << "software bug that you should report to Centreon Broker "
	 << "developers");
  return ;
}

/**
 *  Update the stream.
 */
void stream::update() {
  if (!_passive)
    _pblsh.reset(new multiplexing::publisher);
  _save_persistent_cache();
  _load_correlation();
  return ;
}

/**
 *  Write data to the correlation stream.
 *
 *  @param[in] d  Multiplexed data.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data can be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
	   << "correlation stream is shutdown");

  if (d.isNull())
    return (1);

  if (d->type() == neb::host_status::static_type()) {
    neb::host_status const& hs = d.ref_as<neb::host_status>();
    QPair<unsigned int, unsigned int> id(hs.host_id, 0);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end())
      found->manage_status(
        hs.last_hard_state,
        hs.last_hard_state_change,
        _pblsh.get());
  }
  else if (d->type() == neb::service_status::static_type()) {
    neb::service_status const& ss = d.ref_as<neb::service_status>();
    QPair<unsigned int, unsigned int> id(ss.host_id, ss.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end())
      multiplexing::publisher pblsh;
      found->manage_status(
        ss.last_hard_state,
        ss.last_hard_state_change,
        _pblsh.get());
  }
  else if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack
      = d.ref_as<neb::acknowledgement>();
    QPair<unsigned int, unsigned int> id(ack.host_id, ack.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end())
      found->manage_ack(ack, _pblsh.get());
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = d.ref_as<neb::downtime>();
    QPair<unsigned int, unsigned int> id(dwn.host_id, dwn.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end())
      found->manage_downtime(dwn, _pblsh.get());
  }
  else if (d->type() == neb::log_entry::static_type()) {
    neb::log_entry const& entry = d.ref_as<neb::log_entry>();
    QPair<unsigned int, unsigned int> id(entry.host_id, entry.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end())
      found->manage_log(entry, _pblsh.get());
  }
  else if (d->type() == neb::downtime_removed::static_type()) {
    neb::downtime_removed const& dr
      = d.ref_as<neb::downtime_removed>();
    QPair<unsigned int, unsigned int> id(dr.host_id, dr.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end())
      found->manage_downtime_removed(dr.downtime_id, _pblsh.get());
  }
  return (1);
}

/**
 *  Set the state.
 *
 *  @param[in] st  the state.
 */
void stream::set_state(
       QMap<QPair<unsigned int, unsigned int>, node> const& st) {
  _nodes = st;
}

/**
 *  Get the state.
 *
 *  @return  The state.
 */
QMap<QPair<unsigned int, unsigned int>, node> const&
  stream::get_state() const {
  return (_nodes);
}

/**
 *  Load correlation from the configuration file and the persistent
 *  cache.
 */
void stream::_load_correlation() {
  parser p;
  p.parse(_correlation_file, _nodes);

  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  // Load the cache.
  misc::shared_ptr<io::data> d;
  while (true) {
    _cache->get(d);
    if (d.isNull())
      break ;
    _load_correlation_event(d);
  }
}

/**
 *  Load a correlation event from the cache.
 *
 *  @param[in] d  The event.
 */
void stream::_load_correlation_event(misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return ;

  if (d->type() == issue::static_type()) {
    issue const& iss = d.ref_as<issue>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(iss.host_id, iss.service_id));
    if (found != _nodes.end())
      found->my_issue.reset(new issue(iss));
  }
  else if (d->type() == state::static_type()) {
    state const& st = d.ref_as<state>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(st.host_id, st.service_id));
    if (found != _nodes.end()) {
      found->my_state.reset(new state(st));
      found->state = st.current_state;
    }
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = d.ref_as<neb::downtime>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(dwn.host_id, dwn.service_id));
    if (found != _nodes.end())
      found->manage_downtime(dwn, NULL);
  }
  else if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack = d.ref_as<neb::acknowledgement>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(ack.host_id, ack.service_id));
    if (found != _nodes.end())
      found->manage_ack(ack, NULL);
  }
}

/**
 *  Save content of the correlation memory in the persistent cache.
 */
void stream::_save_persistent_cache() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  // Serialize to the cache.
  _cache->transaction();
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it = _nodes.begin(),
         end = _nodes.end();
       it != end;
       ++it)
    it->serialize(*_cache);
  _cache->commit();
}
