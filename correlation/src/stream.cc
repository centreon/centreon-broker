/*
** Copyright 2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Constructor.
 *
 *  @param[in]      correlation_file  Correlation file.
 *  @param[int,out] cache             Persistent cache.
 *  @param[in]      load_correlation  True if we should load correlation
 *                                    state from persistent cache.
 *  @param[in]      passive           Is this stream passive ?
 *                                    (won't send any event)
 */
stream::stream(
          QString const& correlation_file,
          misc::shared_ptr<persistent_cache> cache,
          bool load_correlation,
          bool passive)
  : _cache(cache),
    _correlation_file(correlation_file) {
  if (!passive) {
    // Events will be written to publisher.
    _pblsh.reset(new multiplexing::publisher);

    // Create the engine started event.
    misc::shared_ptr<engine_state> es(new engine_state);
    es->poller_id = config::applier::state::instance().poller_id();
    es->started = true;
    _pblsh->write(es);
  }

  // Load the correlation.
  if (load_correlation)
    _load_correlation();
}

/**
 *  Destructor.
 */
stream::~stream() {
  try {
    if (_pblsh.get()) {
      misc::shared_ptr<engine_state> es(new engine_state);
      es->poller_id = config::applier::state::instance().poller_id();
      _pblsh->write(es);
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "correlator: error while trying to publish engine state: "
      << e.what();
  }
}

/**
 *  Read data from the stream.
 *
 *  @param[out] d         Unused.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method throws.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from correlation stream");
  return (true);
}

/**
 *  Update the stream.
 */
void stream::update() {
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
  if (d.isNull())
    return (1);

  if (d->type() == neb::host_status::static_type()) {
    neb::host_status const& hs = d.ref_as<neb::host_status>();
    QPair<unsigned int, unsigned int> id(hs.host_id, 0);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: processing host status " << hs.last_hard_state
        << " for node (" << hs.host_id << ", 0)";
      found->manage_status(
        hs.last_hard_state,
        hs.last_hard_state_change,
        _pblsh.get());
    }
  }
  else if (d->type() == neb::service_status::static_type()) {
    neb::service_status const& ss = d.ref_as<neb::service_status>();
    QPair<unsigned int, unsigned int> id(ss.host_id, ss.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: processing service status "  << ss.last_hard_state
        << " for node (" << ss.host_id << ", " << ss.service_id << ")";
      found->manage_status(
        ss.last_hard_state,
        ss.last_hard_state_change,
        _pblsh.get());
    }
  }
  else if (d->type() == neb::host::static_type()) {
    neb::host const& h(d.ref_as<neb::host>());
    QPair<unsigned int, unsigned int> id(h.host_id, 0);
    QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
      it(_nodes.find(id));
    if ((it != _nodes.end()) && _pblsh.get()) {
      logging::debug(logging::medium)
        << "correlation: generating state event for host "
        << h.host_id << " following its (re)declaration";
      _pblsh->write(new state(*it));
    }
  }
  else if (d->type() == neb::service::static_type()) {
    neb::service const& s(d.ref_as<neb::service>());
    QPair<unsigned int, unsigned int> id(s.host_id, s.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
      it(_nodes.find(id));
    if ((it != _nodes.end()) && _pblsh.get()) {
      logging::debug(logging::medium)
        << "correlation: generating state event for service ("
        << s.host_id << ", " << s.service_id
        << ") following its (re)declaration";
      _pblsh->write(new state(*it));
    }
  }
  else if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack
      = d.ref_as<neb::acknowledgement>();
    QPair<unsigned int, unsigned int> id(ack.host_id, ack.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: processing ack for node ("
        << ack.host_id << ", " << ack.service_id << ")";
      found->manage_ack(ack, _pblsh.get());
    }
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = d.ref_as<neb::downtime>();
    QPair<unsigned int, unsigned int> id(dwn.host_id, dwn.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: processing downtime ("
	<< dwn.actual_start_time << "-" << dwn.actual_end_time
	<< ") for node ("
        << dwn.host_id << ", " << dwn.service_id << ")";
      found->manage_downtime(dwn, _pblsh.get());
    }
  }
  else if (d->type() == neb::log_entry::static_type()) {
    neb::log_entry const& entry = d.ref_as<neb::log_entry>();
    QPair<unsigned int, unsigned int> id(entry.host_id, entry.service_id);
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(id);
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: processing log for node ("
        << entry.host_id << ", " << entry.service_id << ")";
      found->manage_log(entry, _pblsh.get());
    }
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

  // Load the cache.
  if (!_cache.isNull()) {
    misc::shared_ptr<io::data> d;
    while (true) {
      _cache->get(d);
      if (d.isNull())
        break ;
      _load_correlation_event(d);
    }
  }

  return ;
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
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: loading initial issue for node ("
        << iss.host_id << ", " << iss.service_id << ")";
      found->my_issue.reset(new issue(iss));
    }
  }
  else if (d->type() == state::static_type()) {
    state const& st = d.ref_as<state>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(st.host_id, st.service_id));
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: loading initial state for node ("
        << st.host_id << ", " << st.service_id << ")";
      *static_cast<state*>(&*found) = st;
    }
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = d.ref_as<neb::downtime>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(dwn.host_id, dwn.service_id));
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: loading initial downtime for node ("
        << dwn.host_id << ", " << dwn.service_id << ")";
      found->manage_downtime(dwn, NULL);
    }
  }
  else if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack = d.ref_as<neb::acknowledgement>();
    QMap<QPair<unsigned int, unsigned int>, node>::iterator found
      = _nodes.find(qMakePair(ack.host_id, ack.service_id));
    if (found != _nodes.end()) {
      logging::debug(logging::medium)
        << "correlation: loading initial acknowledgement for node ("
        << ack.host_id << ", " << ack.service_id << ")";
      found->manage_ack(ack, NULL);
    }
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
