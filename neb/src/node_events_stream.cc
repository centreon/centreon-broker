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

#include <cstdio>
#include <QPair>
#include <QHash>
#include "com/centreon/broker/neb/tokenizer.hh"
#include "com/centreon/broker/neb/node_id.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/command_file/external_command.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/neb/timeperiod_loader.hh"
#include "com/centreon/broker/neb/composed_timeperiod_builder.hh"
#include "com/centreon/broker/neb/timeperiod_by_name_builder.hh"
#include "com/centreon/broker/neb/timeperiod_linker.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Constructor.
 *
 *  @param[int,out] cache             Persistent cache.
 *  @param[in]      with_timeperiods  Should the stream try to load timeperiods?
 *  @param[in]      conf              Database configuration.
 */
node_events_stream::node_events_stream(
    misc::shared_ptr<persistent_cache> cache,
    bool with_timeperiods,
    database_config const& conf)
  : _cache(cache),
    _conf(conf),
    _process_out(true),
    _with_timeperiods(with_timeperiods),
    _actual_downtime_id(0) {

  if (_with_timeperiods) {
    // Load the timeperiods.
    _load_timeperiods();
    // Check tp coherency.
    _check_downtime_timeperiod_consistency();
  }

  // Load the cache.
  _load_cache();

  // Start the downtime scheduler.
  _downtime_scheduler.start_and_wait();
}

/**
 *  Destructor.
 */
node_events_stream::~node_events_stream() {
  try {
    // Save the cache.
    _save_cache();

    // Stop the downtime scheduler.
    _downtime_scheduler.quit();
    _downtime_scheduler.wait();
  } catch (std::exception const& e) {
    logging::error(logging::medium)
      << "neb: node events error while trying to save cache: "
      << e.what();
  }
}

/**
 *  Set which data to process.
 *
 *  @param[in] in   Process in.
 *  @param[in] out  Process out.
 */
void node_events_stream::process(bool in, bool out) {
  (void)in;
  _process_out = out;
  return ;
}

/**
 *  Read data from the stream.
 *
 *  @param[out] d         Unused.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Always throw.
 */
bool node_events_stream::read(
                           misc::shared_ptr<io::data>& d,
                           time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from a node events stream");
  return (true);
}

/**
 *  Update the stream.
 */
void node_events_stream::update() {
  _save_cache();

  if (_with_timeperiods) {
    // Load the timeperiods.
    _load_timeperiods();
    // Check tp coherency.
    _check_downtime_timeperiod_consistency();
  }

  return ;
}

/**
 *  Write data to the correlation stream.
 *
 *  @param[in] d  Multiplexed data.
 */
unsigned int node_events_stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data can be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
     << "correlation stream is shutdown");

  if (d.isNull())
    return (1);

  // Manage data.
  if (d->type() == neb::host_status::static_type()) {
    _process_host_status(d.ref_as<neb::host_status const>());
  }
  else if (d->type() == neb::service_status::static_type()) {
    _process_service_status(d.ref_as<neb::service_status const>());
  }
  else if (d->type() == neb::downtime::static_type()) {
    _update_downtime(d.ref_as<neb::downtime const>());
  }
  else if (d->type() == command_file::external_command::static_type()) {
    try {
      multiplexing::publisher pblsh;
      parse_command(d.ref_as<command_file::external_command const>(), pblsh);
    } catch (std::exception const& e) {
      logging::error(logging::medium)
        << "neb: node events stream can't parse command '"
        << d.ref_as<command_file::external_command>().command
        << "': " << e.what();
    }
  }

  // Write to the node cache.
  _node_cache.write(d);

  return (1);
}

/**
 *  This class holds a RAII buffer for parsing.
 */
class buffer {
public:
  explicit buffer(unsigned int size) {
    _data = new char[size];
  }
  buffer(buffer const& buf) {
    _data = ::strdup(buf._data);
  }
  buffer& operator=(buffer const& buf) {
    if (this != &buf)
      _data = ::strdup(buf._data);
    return (*this);
  }
  ~buffer() {
    delete [] _data;
  }
  bool operator==(const char* other) const {
    return (::strcmp(_data, other) == 0);
  }
  char* get() {
    return (_data);
  }
  const char* get() const {
    return (_data);
  }

private:
  char* _data;
};

/**
 *  Parse an external command.
 *
 *  @param[in] exc     External command.
 *  @param[in] stream  Output stream.
 *
 *  @return         An event.
 */
void node_events_stream::parse_command(
                          command_file::external_command const& exc,
                          io::stream& stream) {
  std::string line = exc.command.toStdString();
  buffer command(line.size());
  buffer args(line.size());

  logging::debug(logging::low)
    << "neb: node events stream received command: '" << exc.command << "'";

  // Parse timestamp.
  unsigned long timestamp;
  if (::sscanf(
        line.c_str(),
        "[%lu] %[^ ;];%[^\n]",
        &timestamp,
        command.get(),
        args.get()) != 3)
    throw (exceptions::msg()
           << "couldn't parse the line");

  if (command == "ACKNOWLEDGE_HOST_PROBLEM")
    _parse_ack(ack_host, timestamp, args.get(), stream);
  else if (command == "ACKNOWLEDGE_SVC_PROBLEM")
    _parse_ack(ack_service, timestamp, args.get(), stream);
  else if (command == "REMOVE_HOST_ACKNOWLEDGEMENT")
    _parse_remove_ack(ack_host, timestamp, args.get(), stream);
  else if (command == "REMOVE_SVC_ACKNOWLEDGEMENT")
    _parse_remove_ack(ack_service, timestamp, args.get(), stream);
  else if (command == "SCHEDULE_HOST_DOWNTIME")
    _parse_downtime(down_host, timestamp, args.get(), stream);
  else if (command == "SCHEDULE_HOST_SVC_DOWNTIME")
    _parse_downtime(down_host_service, timestamp, args.get(), stream);
  else if (command == "SCHEDULE_SVC_DOWNTIME")
    _parse_downtime(down_service, timestamp, args.get(), stream);
  else if (command == "DELETE_HOST_DOWNTIME")
    _parse_remove_downtime(down_host, timestamp, args.get(), stream);
  else if (command == "DELETE_SVC_DOWNTIME")
    _parse_remove_downtime(down_service, timestamp, args.get(), stream);
}

/**
 *  Set the timeperiods.
 *
 *  Used for testing purpos.
 *
 *  @param[in] tps  The timeperiods.
 */
void node_events_stream::set_timeperiods(
        QHash<QString, time::timeperiod::ptr> const& tps) {
  _timeperiods = tps;
}

/**
 *  Process a host status event.
 *
 *  @param[in] hst  Host status event.
 */
void node_events_stream::_process_host_status(
                           neb::host_status const& hst) {
  node_id id(hst.host_id);
  short prev_state = _node_cache.get_current_state(id);
  _remove_expired_acknowledgement(id, prev_state, hst.last_hard_state);
  _trigger_floating_downtime(
    id,
    hst.last_hard_state_change,
    hst.last_hard_state);
}

/**
 *  Process a service status event.
 *
 *  @param[in] sst  Service status event.
 */
void node_events_stream::_process_service_status(
                           neb::service_status const& sst) {
  node_id id(sst.host_id, sst.service_id);
  short prev_state = _node_cache.get_current_state(id);
  _remove_expired_acknowledgement(id, prev_state, sst.last_hard_state);
  _trigger_floating_downtime(
    id,
    sst.last_hard_state_change,
    sst.last_hard_state);
}

/**
 *  @brief Update a downtime from information received.
 *
 *  The downtime scheduler can send downtime periodically when
 *  downtimes start or stop. We need to update the downtimes we have.
 *
 *  @param[in] dwn  The downtime.
 */
void node_events_stream::_update_downtime(
                           neb::downtime const& dwn) {
  QHash<unsigned int, neb::downtime>::iterator
    found(_downtimes.find(dwn.internal_id));

  // No downtimes, insert it.
  if (found == _downtimes.end())
    found = _downtimes.insert(dwn.internal_id, dwn);

  // Downtime update.
  downtime& old_downtime = *found;
  old_downtime = dwn;

  // Downtime removal.
  if (!dwn.actual_end_time.is_null()) {
    _downtimes.erase(found);
    _downtime_id_by_nodes.remove(
      node_id(dwn.host_id, dwn.service_id),
      dwn.internal_id);
    // Recurring downtimes.
    if (dwn.triggered_by != 0
          && _recurring_downtimes.contains(dwn.triggered_by))
      _spawn_recurring_downtime(
        dwn.actual_end_time + dwn.recurring_interval,
        _recurring_downtimes[dwn.triggered_by]);
  }
}

/**
 *  Remove an expired acknowledgement.
 *
 *  @param[in] node        A node.
 *  @param[in] prev_state  Its previous state.
 *  @param[in] state       Its current state.
 */
void node_events_stream::_remove_expired_acknowledgement(
                           node_id node,
                           short prev_state,
                           short state) {
  QHash<node_id, neb::acknowledgement>::iterator found
    = _acknowledgements.find(node);
  if (found != _acknowledgements.end()
        && (state == 0 || (!found->is_sticky && prev_state != state))) {
    // Close the ack.
    found->deletion_time = state;
    multiplexing::publisher pblsh;
    pblsh.write(misc::make_shared(new neb::acknowledgement(*found)));
    _acknowledgements.erase(found);
  }
}

/**
 *  Trigger all the floating downtime of a node.
 *
 *  @param[in] node        A node.
 *  @param[in] check_time  The time of its last check.
 *  @param[in] prev_state  Its previous state.
 *  @param[in] state       Its current state.
 */
void node_events_stream::_trigger_floating_downtime(
                           node_id node,
                           timestamp check_time,
                           short state) {
  if (state == 0)
    return ;
  for (QMultiHash<node_id, unsigned int>::iterator
         it = _downtime_id_by_nodes.find(node),
         tmp = it,
         end = _downtime_id_by_nodes.end();
       it != end && it.key() == node;
       it = tmp) {
    ++tmp;
    // Trigger downtimes not already triggered.
    downtime const& dwn = _downtimes[*it];
    if (!dwn.fixed
          && check_time >= dwn.start_time
          && check_time < dwn.end_time
          && dwn.actual_start_time.is_null())
      _downtime_scheduler.add_downtime(
        check_time,
        check_time + dwn.duration,
        dwn);
    // Remove expired non-triggered downtimes.
    if (!dwn.fixed
          && check_time >= dwn.end_time
          && dwn.actual_start_time.is_null()) {
      _downtimes.remove(dwn.internal_id);
      _downtime_id_by_nodes.erase(it);
    }
  }
}

/**
 *  Parse an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             An acknowledgement event.
 */
void node_events_stream::_parse_ack(
                           ack_type is_host,
                           timestamp t,
                           const char* args,
                           io::stream& stream) {
  tokenizer tok(args);

  try {
    // Parse.
    tok.begin();
    std::string host_name = tok.get_next_token<std::string>();
    std::string service_description =
      (is_host == ack_host ? "" : tok.get_next_token<std::string>());
    int sticky = tok.get_next_token<int>();
    int notify = tok.get_next_token<int>();
    int persistent_comment = tok.get_next_token<int>();
    std::string author = tok.get_next_token<std::string>(true);
    std::string comment = tok.get_next_token<std::string>(true);
    tok.end();

    node_id id(_node_cache.get_node_by_names(
                 host_name,
                 service_description));
    misc::shared_ptr<neb::acknowledgement>
      ack(new neb::acknowledgement);
    ack->acknowledgement_type = is_host;
    ack->comment = QString::fromStdString(comment);
    ack->author = QString::fromStdString(author);
    ack->entry_time = t;
    ack->host_id = id.get_host_id();
    ack->service_id = id.get_service_id();
    ack->is_sticky = (sticky == 2);
    ack->persistent_comment = (persistent_comment == 1);
    ack->notify_contacts = (notify == 1 || notify == 2);
    ack->notify_only_if_not_already_acknowledged = (notify == 2);

    // Save acknowledgements.
    _acknowledgements[id] = *ack;

    // Send the acknowledgement.
    stream.write(ack);

  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement: "
           <<  e.what());
  }
}

/**
 *  Parse the removal of an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             An acknowledgement removal event.
 */
void node_events_stream::_parse_remove_ack(
                           ack_type type,
                           timestamp t,
                           const char* args,
                           io::stream& stream) {
  tokenizer tok(args);
  try {
    // Parse.
    tok.begin();
    std::string host_name = tok.get_next_token<std::string>();
    std::string service_description =
      (type == ack_host ? "" : tok.get_next_token<std::string>());
    tok.end();

    // Find the node id from the host name / description.
    node_id id = _node_cache.get_node_by_names(
                   host_name,
                   service_description);

    // Find the ack.
    QHash<node_id, neb::acknowledgement>::iterator
      found(_acknowledgements.find(id));
    if (found == _acknowledgements.end())
      throw (exceptions::msg()
             << "couldn't find an acknowledgement for ("
             << id.get_host_id() << ", " << id.get_service_id() << ")");

    // Close the ack.
    misc::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement(*found));
    ack->deletion_time = t;

    // Erase the ack.
    _acknowledgements.erase(found);

    // Send the closed ack.
    stream.write(ack);
  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement removal: "
           << e.what());
  }
}

/**
 *  Parse a downtime.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             A downtime event.
 */
void node_events_stream::_parse_downtime(
                           down_type type,
                           timestamp t,
                           char const* args,
                           io::stream& stream) {
  tokenizer tok(args);

  (void)t;
  logging::debug(logging::medium)
    << "notification: parsing downtime command: '" << args << "'";

  try {
    // Parse.
    tok.begin();
    std::string host_name =     tok.get_next_token<std::string>();
    std::string service_description =
      (type == down_host ? "" : tok.get_next_token<std::string>());
    unsigned long start_time =  tok.get_next_token<unsigned long>();
    unsigned long end_time =    tok.get_next_token<unsigned long>();
    int fixed =                 tok.get_next_token<int>();
    unsigned int trigger_id =   tok.get_next_token<unsigned int>();
    unsigned int duration =     tok.get_next_token<unsigned int>();
    std::string author =        tok.get_next_token<std::string>(true);
    std::string comment =       tok.get_next_token<std::string>(true);
    std::string recurring_timeperiod =
                                tok.get_next_token<std::string>(true);
    unsigned int recurring_interval =
                                tok.get_next_token<unsigned int>(true);
    tok.end();

    node_id id = _node_cache.get_node_by_names(
                   host_name,
                   service_description);

    misc::shared_ptr<neb::downtime>
      d(new neb::downtime);
    d->author = QString::fromStdString(author);
    d->comment = QString::fromStdString(comment);
    d->start_time = start_time;
    d->end_time = end_time;
    d->duration = fixed ? end_time - start_time : duration;
    d->fixed = (fixed == 1);
    d->downtime_type = type;
    d->host_id = id.get_host_id();
    d->service_id = id.get_service_id();
    d->was_started = false;
    d->internal_id = ++_actual_downtime_id;
    d->is_recurring = recurring_interval != 0;
    d->triggered_by = trigger_id;
    d->recurring_interval = recurring_interval;
    d->recurring_timeperiod = QString::fromStdString(recurring_timeperiod);

    // Save the downtime.
    if (!d->is_recurring) {
      _downtimes[d->internal_id] = *d;
      _downtime_id_by_nodes.insert(id, d->internal_id);
    }
    else {
      _recurring_downtimes[d->internal_id] = *d;
    }

    // Write the downtime.
    stream.write(d);

    // Schedule the downtime.
    if (!d->is_recurring)
      _schedule_downtime(*d);
    else
      _spawn_recurring_downtime(timestamp(), *d);

  } catch (std::exception const& e) {
    throw (exceptions::msg()
           << "error while parsing downtime arguments: " << e.what());
  }
}

/**
 *  Parse a downtime removal.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             A downtime removal event.
 */
void node_events_stream::_parse_remove_downtime(
                           down_type type,
                           timestamp t,
                           const char* args,
                           io::stream& stream) {
  (void)type;
  unsigned int downtime_id;
  if (::sscanf(args, "%u", &downtime_id) != 1)
    throw (exceptions::msg() << "error while parsing remove downtime arguments");

  // Find the downtime.
  QHash<unsigned int, neb::downtime>::iterator
    found(_downtimes.find(downtime_id));
  QHash<unsigned int, neb::downtime>::iterator
    recurring_found(_recurring_downtimes.find(downtime_id));
  if (found == _downtimes.end()
        && recurring_found == _recurring_downtimes.end())
    throw (exceptions::msg()
           << "couldn't find a downtime for downtime id " << downtime_id);

  if (found == _downtimes.end())
    found = recurring_found;

  node_id node(found->host_id, found->service_id);

  // Close the downtime.
  misc::shared_ptr<neb::downtime> d(new neb::downtime(*found));
  d->actual_end_time = t;
  d->deletion_time = t;
  d->was_cancelled = true;

  // Erase the downtime.
  _downtimes.remove(downtime_id);
  _recurring_downtimes.remove(downtime_id);
  _downtime_id_by_nodes.remove(node, downtime_id);
  _downtime_scheduler.remove_downtime(downtime_id);

  // Send the closed downtime.
  stream.write(d);
}

/**
 *  Load the timeperiods from the database.
 */
void node_events_stream::_load_timeperiods() {
  database db(_conf);

  try {
    timeperiod_loader loader;
    composed_timeperiod_builder builder;
    timeperiod_by_name_builder by_name(_timeperiods);
    timeperiod_linker linker;
    builder.push_back(by_name);
    builder.push_back(linker);
    loader.load(&db.get_qt_db(), &builder);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "neb: node event streams:"
           " error while trying to load timeperiods: " << e.what());
  }
}

/**
 *  Check that each recurring timeperiod has its downtime.
 */
void node_events_stream::_check_downtime_timeperiod_consistency() {
  for (QHash<unsigned int, neb::downtime>::iterator
         it = _recurring_downtimes.begin(),
         tmp = it,
         end = _recurring_downtimes.end();
       it != end;
       it = tmp) {
    if (!_timeperiods.contains(it->recurring_timeperiod)) {
      ++tmp;
      logging::error(logging::medium)
        << "core: node events stream: recurring timeperiod '"
        << it->recurring_timeperiod << "' deleted,"
           " deleting associated downtime " << it->internal_id;
      _recurring_downtimes.erase(it);
    }
  }
}

/**
 *  Load the cache.
 */
void node_events_stream::_load_cache() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  misc::shared_ptr<io::data> d;
  while (true) {
    _cache->get(d);
    if (d.isNull())
      break ;
    _process_loaded_event(d);
  }
}

/**
 *  Process an event loaded from the cache.
 *
 *  @param[in] d  The event.
 */
void node_events_stream::_process_loaded_event(
                           misc::shared_ptr<io::data> const& d) {
  // Write to the node cache.
  _node_cache.write(d);

  // Managed internally.
  if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack = d.ref_as<neb::acknowledgement const>();
    _acknowledgements[node_id(ack.host_id, ack.service_id)] = ack;
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = d.ref_as<neb::downtime const>();
    _downtimes[dwn.internal_id] = dwn;
    _downtime_id_by_nodes.insert(
      node_id(dwn.host_id, dwn.service_id), dwn.internal_id);
    if (_actual_downtime_id < dwn.internal_id)
      _actual_downtime_id = dwn.internal_id + 1;
    if (!dwn.is_recurring)
      _schedule_downtime(dwn);
    else
      _spawn_recurring_downtime(
        timestamp(),
        dwn);
  }
}

/**
 *  Save the cache.
 */
void node_events_stream::_save_cache() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  _cache->transaction();
  // Serialize the node cache.
  _node_cache.serialize(_cache);
  // Managed internally.
  for (QHash<node_id, neb::acknowledgement>::const_iterator
         it = _acknowledgements.begin(),
         end = _acknowledgements.end();
       it != end;
       ++it)
    _cache->add(misc::make_shared(new neb::acknowledgement(*it)));
  for (QHash<unsigned int, neb::downtime>::const_iterator
         it = _downtimes.begin(),
         end = _downtimes.end();
       it != end;
       ++it)
    _cache->add(misc::make_shared(new neb::downtime(*it)));
  for (QHash<unsigned int, neb::downtime>::const_iterator
         it = _recurring_downtimes.begin(),
         end = _recurring_downtimes.end();
       it != end;
       ++it)
    _cache->add(misc::make_shared(new neb::downtime(*it)));
  _cache->commit();
}

/**
 *  Schedule a downtime, if applicable.
 *
 *  @param[in] dwn  The downtime to schedule.
 */
void node_events_stream::_schedule_downtime(
                           downtime const& dwn) {
  // If this is a fixed downtime or the node is in a non-okay state, schedule it.
  // If not, then it will be scheduled at the reception of a
  // service/host status event.
  if (dwn.fixed)
    _downtime_scheduler.add_downtime(dwn.start_time, dwn.end_time, dwn);
  else {
    node_id id(dwn.host_id, dwn.service_id);
    if (id.is_host()) {
      neb::host_status* hst = _node_cache.get_host_status(id);
      if (hst != NULL
            && hst->last_hard_state != 0
            && hst->last_hard_state_change >= dwn.start_time
            && hst->last_hard_state_change < dwn.end_time)
        _downtime_scheduler.add_downtime(
                              hst->last_hard_state_change,
                              hst->last_hard_state_change + dwn.duration,
                              dwn);
    }
    else {
      neb::service_status* sst = _node_cache.get_service_status(id);
      if (sst != NULL
            && sst->last_hard_state != 0
            && sst->last_hard_state_change >= dwn.start_time
            && sst->last_hard_state_change < dwn.end_time)
        _downtime_scheduler.add_downtime(
                              sst->last_hard_state_change,
                              sst->last_hard_state_change + dwn.duration,
                              dwn);
    }
  }
}

/**
 *  Spawn a recurring downtime.
 *
 *  @param[in] when  When we should spawn the downtime,
 *                   null for as soon as possible.
 *  @param[in] dwn   The downtime.
 */
void node_events_stream::_spawn_recurring_downtime(
                           timestamp when,
                           downtime const& dwn) {
  // Only spawn if no other downtimes exist.
  for (QHash<unsigned int, neb::downtime>::const_iterator
         it = _downtimes.begin(),
         end = _downtimes.end();
       it != end;
       ++it)
    if (it->triggered_by == dwn.internal_id)
      return ;

  // Spawn a new downtime.
  downtime spawned(dwn);
  spawned.triggered_by = dwn.internal_id;
  spawned.is_recurring = false;
  spawned.internal_id = ++_actual_downtime_id;

  // Get the timeperiod.
  QHash<QString, time::timeperiod::ptr>::const_iterator
    tp = _timeperiods.find(dwn.recurring_timeperiod);

  if (tp == _timeperiods.end()) {
    logging::error(logging::medium)
      << "neb: node events stream: ignoring recurring downtime "
      << dwn.internal_id << ", timeperiod '" << dwn.recurring_timeperiod
      << "' does not exist";
    return ;
  }

  // Get the new start and end time.
  if (when.is_null())
    when = ::time(NULL);
  spawned.start_time = (*tp)->get_next_valid(when);
  spawned.end_time = spawned.start_time + (dwn.end_time - dwn.start_time);

  // Save the downtime.
  _downtimes[spawned.internal_id] = spawned;
  _downtime_id_by_nodes.insert(
    node_id(spawned.host_id, spawned.service_id),
    spawned.internal_id);

  // Send the downtime.
  multiplexing::publisher pblsh;
  pblsh.write(misc::make_shared(new neb::downtime(spawned)));

  // Schedule the downtime.
  _schedule_downtime(spawned);
}
