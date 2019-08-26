/*
** Copyright 2015,2017 Centreon
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

#include <set>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <QPair>
#include <QHash>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/tokenizer.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/ceof/ceof_parser.hh"
#include "com/centreon/broker/ceof/ceof_writer.hh"
#include "com/centreon/broker/ceof/ceof_deserializer.hh"
#include "com/centreon/broker/ceof/ceof_serializer.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/downtime_serializable.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/neb/node_id.hh"
#include "com/centreon/broker/neb/timeperiod_serializable.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::ceof;

/**
 *  Constructor.
 *
 *  @param[int,out] cache             Persistent cache.
 *  @param[in]      config_file       The path of the config file.
 */
node_events_stream::node_events_stream(
                      std::string const& name,
                      std::shared_ptr<persistent_cache> cache,
                      std::string const& config_file)
  : _cache(cache),
    _config_file(config_file),
    _name(name.c_str()) {
  // Load the config file.
  if (!_config_file.empty())
    _load_config_file();

  // Load the cache.
  _load_cache();

  // Apply the config downtimes.
  _apply_config_downtimes();

  // Check downtime consistency.
  _check_downtime_timeperiod_consistency();

  // Start the downtime scheduler.
  _downtime_scheduler.start_and_wait();
}

/**
 *  Destructor.
 */
node_events_stream::~node_events_stream() {
  try {
    // Stop the downtime scheduler.
    _downtime_scheduler.quit();
    _downtime_scheduler.wait();

    // Save the cache.
    _save_cache();
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "node events: error while trying to save cache: "
      << e.what();
  }
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
                           std::shared_ptr<io::data>& d,
                           time_t deadline) {
  (void)deadline;
  d.reset();
  throw (exceptions::shutdown()
         << "cannot read from a node events stream");
  return true;
}

/**
 *  Update the stream.
 */
void node_events_stream::update() {
  if (!_config_file.empty())
    _load_config_file();
  _apply_config_downtimes();
  _check_downtime_timeperiod_consistency();
  try {
    _save_cache();
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "node events: error while trying to save cache: "
      << e.what();
  }

  return ;
}

/**
 *  Write data to the correlation stream.
 *
 *  @param[in] d  Multiplexed data.
 */
int node_events_stream::write(std::shared_ptr<io::data> const& d) {
  // Check that data can be processed.
  if (!validate(d, "node events"))
    return (1);

  // Manage data.
  if (d->type() == neb::host_status::static_type()) {
    _process_host_status(*std::static_pointer_cast<neb::host_status const>(d));
  }
  else if (d->type() == neb::service_status::static_type()) {
    _process_service_status(*std::static_pointer_cast<neb::service_status const>(d));
  }
  else if (d->type() == neb::downtime::static_type()) {
    _update_downtime(*std::static_pointer_cast<neb::downtime const>(d));
  }
  else if (d->type() == extcmd::command_request::static_type()) {
    extcmd::command_request const&
      req(*std::static_pointer_cast<extcmd::command_request const>(d));
    if (req.is_addressed_to(_name)) {
      multiplexing::publisher pblsh;
      try {
        // Execute command that was especially addressed to us.
        parse_command(req, pblsh);

        // Send successful result.
        std::shared_ptr<extcmd::command_result>
          res(new extcmd::command_result);
        res->uuid = req.uuid;
        res->msg = "\"Command successfully executed.\"";
        res->code = 0;
        res->destination_id = req.source_id;
        pblsh.write(res);
      }
      catch (std::exception const& e) {
        // Log error.
        logging::error(logging::medium)
          << "node events: unable to process command '"
          << req.cmd << "': " << e.what();

        // Send error result.
        std::shared_ptr<extcmd::command_result>
          res(new extcmd::command_result);
        res->uuid = req.uuid;
        res->msg = std::string("\"") + e.what() + "\"";
        res->code = -1;
        res->destination_id = req.source_id;
        pblsh.write(res);
      }
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
 *  Parse an external command request.
 *
 *  @param[in] exc     External command.
 *  @param[in] stream  Output stream.
 *
 *  @return         An event.
 */
void node_events_stream::parse_command(
                           extcmd::command_request const& exc,
                           io::stream& stream) {
  std::string line{exc.cmd};
  buffer command{static_cast<unsigned int>(line.size())};
  buffer args(line.size());

  logging::info(logging::medium)
    << "node events: received command '" << exc.cmd << "'";

  // Parse command and arguments.
  if (::sscanf(
        line.c_str(),
        "%[^ ;];%[^\n]",
        command.get(),
        args.get()) != 2)
    throw (exceptions::msg() << "invalid format: expected"
           << " format is <CMD>[;<ARG1>[;<ARG2>...]]");

  // Execute command.
  if (command == "ACKNOWLEDGE_HOST_PROBLEM")
    _parse_ack(ack_host, args.get(), stream);
  else if (command == "ACKNOWLEDGE_SVC_PROBLEM")
    _parse_ack(ack_service, args.get(), stream);
  else if (command == "REMOVE_HOST_ACKNOWLEDGEMENT")
    _parse_remove_ack(ack_host, args.get(), stream);
  else if (command == "REMOVE_SVC_ACKNOWLEDGEMENT")
    _parse_remove_ack(ack_service, args.get(), stream);
  else if (command == "SCHEDULE_HOST_DOWNTIME")
    _parse_downtime(down_host, args.get(), stream);
  else if (command == "SCHEDULE_HOST_SVC_DOWNTIME")
    _parse_downtime(down_host_service, args.get(), stream);
  else if (command == "SCHEDULE_SVC_DOWNTIME")
    _parse_downtime(down_service, args.get(), stream);
  else if (command == "DEL_HOST_DOWNTIME")
    _parse_remove_downtime(down_host, args.get(), stream);
  else if (command == "DEL_SVC_DOWNTIME")
    _parse_remove_downtime(down_service, args.get(), stream);
  else
    throw (exceptions::msg() << "unknown command: refer to"
           << " documentation for the list of valid commands");

  return ;
}

/**
 *  Set the timeperiods.
 *
 *  Used for testing purpos.
 *
 *  @param[in] tps  The timeperiods.
 */
void node_events_stream::set_timeperiods(
        std::unordered_map<std::string, time::timeperiod::ptr> const& tps) {
  _timeperiods = tps;
}

/**
 *  Process a host status event.
 *
 *  @param[in] hst  Host status event.
 */
void node_events_stream::_process_host_status(
                           neb::host_status const& hst) {
  logging::debug(logging::low)
    << "node events: processing host status for ("
    << hst.host_id << "), state '" << hst.last_hard_state << "'";
  node_id id(hst.host_id);
  short prev_state = _node_cache.get_current_state(id);
  _remove_expired_acknowledgement(
    id,
    hst.last_hard_state_change,
    prev_state,
    hst.last_hard_state);
  _trigger_floating_downtime(
    id,
    hst.last_hard_state);
}

/**
 *  Process a service status event.
 *
 *  @param[in] sst  Service status event.
 */
void node_events_stream::_process_service_status(
                           neb::service_status const& sst) {
  logging::debug(logging::low)
    << "node events: processing host status for ("
    << sst.host_id << ", " << sst.service_id << "), state '"
    << sst.last_hard_state << "'";
  node_id id(sst.host_id, sst.service_id);
  short prev_state = _node_cache.get_current_state(id);
  _remove_expired_acknowledgement(
    id,
    sst.last_hard_state_change,
    prev_state,
    sst.last_hard_state);
  _trigger_floating_downtime(
    id,
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
  downtime* found = _downtimes.get_downtime(dwn.internal_id);

  // No downtimes, insert it.
  if (!found) {
    _downtimes.add_downtime(dwn);
    found = _downtimes.get_downtime(dwn.internal_id);
  }

  // Downtime update.
  downtime& old_downtime = *found;
  old_downtime = dwn;

  // Downtime removal.
  if (!dwn.actual_end_time.is_null()) {
    _downtimes.delete_downtime(dwn);
    // Recurring downtimes.
    if (dwn.triggered_by != 0
        && _downtimes.is_recurring(dwn.triggered_by))
      _spawn_recurring_downtime(
        dwn.actual_end_time,
        *_downtimes.get_downtime(dwn.triggered_by));
  }
}

/**
 *  Remove an expired acknowledgement.
 *
 *  @param[in] node        A node.
 *  @param[in] check_time  The time of the check.
 *  @param[in] prev_state  Its previous state.
 *  @param[in] state       Its current state.
 */
void node_events_stream::_remove_expired_acknowledgement(
                           node_id node,
                           timestamp check_time,
                           short prev_state,
                           short state) {
  std::unordered_map<node_id, neb::acknowledgement>::iterator found{
      _acknowledgements.find(node)};
  if (found != _acknowledgements.end()
        && (state == 0 || (!found->second.is_sticky && prev_state != state))) {
    logging::info(logging::medium)
      << "node events: removing expired acknowledgement for "
         "(" << node.get_host_id() << ", " << node.get_service_id() << ")";
    // Close the ack.
    found->second.deletion_time = check_time;
    multiplexing::publisher pblsh;
    pblsh.write(std::make_shared<neb::acknowledgement>(found->second));
    _acknowledgements.erase(found);
  }
}

/**
 *  Trigger all the floating downtime of a node.
 *
 *  @param[in] node        A node.
 *  @param[in] state       Its current state.
 */
void node_events_stream::_trigger_floating_downtime(
                           node_id node,
                           short state) {
  if (state == 0)
    return ;
  std::list<downtime> downtimes{_downtimes.get_all_downtimes_of_node(node)};
  for (std::list<downtime>::const_iterator
         it = downtimes.begin(),
         end = downtimes.end();
       it != end;
       ++it) {
    downtime const& dwn = *it;
    time_t now(::time(NULL));
    // Trigger downtimes not already triggered.
    if (!dwn.fixed
        && now >= dwn.start_time
        && now < dwn.end_time
        && dwn.actual_start_time.is_null())
      _downtime_scheduler.add_downtime(
        now,
        now + dwn.duration,
        dwn);
    // Remove expired non-triggered downtimes.
    if (!dwn.fixed
        && now >= dwn.end_time
        && dwn.actual_start_time.is_null()) {
      _downtimes.delete_downtime(dwn);
    }
  }
}

/**
 *  Parse an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             An acknowledgement event.
 */
void node_events_stream::_parse_ack(
                           ack_type is_host,
                           const char* args,
                           io::stream& stream) {
  logging::debug(logging::medium)
    << "node events: "
       "parsing acknowledgement command: '" << args << "'";

  misc::tokenizer tok(args);

  try {
    // Parse.
    std::string host_name = tok.get_next_token<std::string>();
    std::string service_description =
      (is_host == ack_host ? "" : tok.get_next_token<std::string>());
    int sticky = tok.get_next_token<int>();
    int notify = tok.get_next_token<int>();
    int persistent_comment = tok.get_next_token<int>();
    std::string author = tok.get_next_token<std::string>(true);
    std::string comment = tok.get_next_token<std::string>(true);

    node_id id(_node_cache.get_node_by_names(
                 host_name,
                 service_description));
    if (id.empty())
      throw (exceptions::msg() << "couldn't find node "
             << host_name << ", " << service_description);

    // The entry time is inherited from any existing ack.
    timestamp t = ::time(NULL);
    std::unordered_map<node_id, neb::acknowledgement>::const_iterator found{
      _acknowledgements.find(id)};
    if (found != _acknowledgements.end())
      t = _acknowledgements[id].entry_time;


    std::shared_ptr<neb::acknowledgement>
      ack(new neb::acknowledgement);
    ack->acknowledgement_type = is_host;
    ack->comment = comment;
    ack->author = author;
    ack->entry_time = t;
    ack->host_id = id.get_host_id();
    ack->service_id = id.get_service_id();
    ack->is_sticky = sticky;
    ack->persistent_comment = (persistent_comment == 1);
    ack->notify_contacts = (notify == 1 || notify == 2);
    ack->notify_only_if_not_already_acknowledged = (notify == 2);

    // Save acknowledgements.
    _acknowledgements[id] = *ack;

    // Send the acknowledgement.
    logging::info(logging::high)
      << "node events: sending ack for "
         "(" << ack->host_id << ", " << ack->service_id << ")";
    stream.write(ack);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement: "
           <<  e.what());
  }
}

/**
 *  Parse the removal of an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             An acknowledgement removal event.
 */
void node_events_stream::_parse_remove_ack(
                           ack_type type,
                           const char* args,
                           io::stream& stream) {
  logging::debug(logging::medium)
    << "node events: "
       "parsing acknowledgement removal command: '" << args << "'";

  misc::tokenizer tok(args);
  try {
    // Parse.
    std::string host_name = tok.get_next_token<std::string>();
    std::string service_description =
      (type == ack_host ? "" : tok.get_next_token<std::string>());

    // Find the node id from the host name / description.
    node_id id = _node_cache.get_node_by_names(
                   host_name,
                   service_description);

    // Find the ack.
    std::unordered_map<node_id, neb::acknowledgement>::iterator
      found(_acknowledgements.find(id));
    if (found == _acknowledgements.end())
      throw (exceptions::msg()
             << "couldn't find an acknowledgement for ("
             << id.get_host_id() << ", " << id.get_service_id() << ")");

    // Close the ack.
    std::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement(found->second));
    ack->deletion_time = ::time(NULL);

    // Erase the ack.
    _acknowledgements.erase(found);

    // Send the closed ack.
    logging::info(logging::high)
      << "node events: erasing acknowledgement for "
         "(" << ack->host_id << ", " << ack->service_id << ")";
    stream.write(ack);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement removal: "
           << e.what());
  }
}

/**
 *  Parse a downtime.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             A downtime event.
 */
void node_events_stream::_parse_downtime(
                           down_type type,
                           char const* args,
                           io::stream& stream) {
  misc::tokenizer tok(args);

  logging::debug(logging::medium)
    << "node events: parsing downtime command '" << args << "'";

  try {
    // Parse.
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

    node_id id = _node_cache.get_node_by_names(
                   host_name,
                   service_description);
    if (id.empty())
      throw (exceptions::msg() << "couldn't find node "
             << host_name << ", " << service_description);

    std::shared_ptr<neb::downtime>
      d(new neb::downtime);
    d->author = author;
    d->comment = comment;
    d->start_time = start_time;
    d->end_time = end_time;
    d->entry_time = ::time(nullptr);
    d->duration = fixed ? end_time - start_time : duration;
    d->fixed = (fixed == 1);
    d->downtime_type = type;
    d->host_id = id.get_host_id();
    d->service_id = id.get_service_id();
    d->poller_id = config::applier::state::instance().poller_id();
    d->was_started = false;
    d->internal_id = _downtimes.get_new_downtime_id();
    d->triggered_by = trigger_id;
    d->recurring_timeperiod = recurring_timeperiod;
    d->is_recurring = !d->recurring_timeperiod.empty();
    d->entry_time = ::time(NULL);

    logging::info(logging::high)
      << "node events: sending downtime for "
         "(" << d->host_id << ", " << d->service_id << ")";

    _register_downtime(*d, &stream);

  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "error while parsing downtime arguments: " << e.what());
  }
}

/**
 *  Parse a downtime removal.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] args     The args to parse.
 *  @param[in] stream   The output stream.
 *
 *  @return             A downtime removal event.
 */
void node_events_stream::_parse_remove_downtime(
                           down_type type,
                           const char* args,
                           io::stream& stream) {
  (void)type;
  unsigned int downtime_id;

  logging::debug(logging::medium)
    << "node events: parsing downtime removal command '" << args << "'";

  if (::sscanf(args, "%u", &downtime_id) != 1)
    throw (exceptions::msg() << "error while parsing remove downtime arguments");

  // Find the downtime.
  downtime* found = _downtimes.get_downtime(downtime_id);
  if (!found)
    throw (exceptions::msg()
           << "couldn't find a downtime for downtime id " << downtime_id);

  logging::info(logging::high)
    << "node events: erasing downtime '" << downtime_id << "'";

  _delete_downtime(*found, ::time(NULL), &stream);
}

/**
 *  Register a downtime.
 *
 *  @param[in] dwn     The downtime.
 *  @param[in] stream  The stream to send the downtime.
 */
void node_events_stream::_register_downtime(downtime const& dwn, io::stream* stream) {

  // Save the downtime.
  _downtimes.add_downtime(dwn);

  // Write the downtime.
  if (stream)
    stream->write(std::make_shared<downtime>(dwn));

  // Schedule the downtime.
  if (!dwn.is_recurring)
    _schedule_downtime(dwn);
  else
    _spawn_recurring_downtime(timestamp(), dwn);
}

/**
 *  Delete a registered a downtime.
 *
 *  @param[in] dwn     The downtime.
 *  @param[in] ts      When the downtime was deleted.
 *  @param[in] stream  The stream to send the deleted downtime event.
 */
void node_events_stream::_delete_downtime(
                           downtime const& dwn,
                           timestamp ts,
                           io::stream* stream) {
  unsigned int downtime_id = dwn.internal_id;
  node_id node(dwn.host_id, dwn.service_id);

  // Close the downtime.
  std::shared_ptr<neb::downtime> d(new neb::downtime(dwn));
  d->actual_end_time = ts;
  d->deletion_time = ts;
  d->was_cancelled = true;

  // Erase the downtime.
  _downtimes.delete_downtime(dwn);
  _downtime_scheduler.remove_downtime(downtime_id);

  // Send the closed downtime.
  if (stream)
    stream->write(d);

  // If we erased a spawned downtime, respawn it.
    if (_downtimes.is_recurring(dwn.triggered_by))
      _spawn_recurring_downtime(
        dwn.deletion_time,
        *_downtimes.get_downtime(dwn.triggered_by));
}

/**
 *  Check that each recurring timeperiod has its downtime.
 */
void node_events_stream::_check_downtime_timeperiod_consistency() {
  std::list<downtime> recurring_downtimes{
      _downtimes.get_all_recurring_downtimes()};
  for (std::list<downtime>::const_iterator
         it{recurring_downtimes.begin()},
         end{recurring_downtimes.end()};
       it != end;
       ++it) {
    if (!_timeperiods.count(it->recurring_timeperiod)) {
      logging::error(logging::medium)
        << "core: node events stream: recurring timeperiod '"
        << it->recurring_timeperiod << "' deleted,"
           " deleting associated downtime " << it->internal_id;
      _downtimes.delete_downtime(*it);
    }
  }
}

/**
 *  Load the config file.
 */
void node_events_stream::_load_config_file() {
  // Open and get the file in memory.
  std::stringstream ss;
  std::ifstream ifs;
  try {
    ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    ifs.open(_config_file.c_str());
    ss << ifs.rdbuf();
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "node_events: couldn't load file '"
           << _config_file << "': " << e.what());
  }

  // Load timeperiods and downtimes.
  _incomplete_downtime.clear();
  _timeperiods.clear();
  std::string document = ss.str();
  try  {
    ceof_parser parser(document);
    for (ceof_iterator iterator = parser.parse();
         !iterator.end();
         ++iterator) {
      std::string const& object_name = iterator.get_value();
      if (object_name == "downtime") {
        downtime_serializable ds;
        ceof::ceof_deserializer cd(iterator.enter_children());
        ds.visit(cd);
        _incomplete_downtime.push_back(*ds.get_downtime());
      }
      else if (object_name == "timeperiod") {
        timeperiod_serializable ts(_timeperiods);
        ceof::ceof_deserializer cd(iterator.enter_children());
        ts.visit(cd);
        _timeperiods.insert(
            {ts.get_name(), ts.get_timeperiod()});
      }
    }
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "node_events: couldn't parse file '"
           << _config_file << "': " << e.what());
  }
}

/**
 *  Load the cache.
 */
void node_events_stream::_load_cache() {
  // No cache, nothing to do.
  if (_cache.get() == NULL)
    return ;

  logging::info(logging::medium) << "node events: loading cache";

  std::shared_ptr<io::data> d;
  while (true) {
    _cache->get(d);
    if (!d)
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
                           std::shared_ptr<io::data> const& d) {
  // Write to the node cache.
  _node_cache.write(d);

  // Managed internally.
  if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack = *std::static_pointer_cast<neb::acknowledgement const>(d);
    logging::debug(logging::medium)
      << "node events: loading acknowledgement for ("
      << ack.host_id << ", " << ack.service_id << ")"
      << ", starting at " << ack.entry_time;
    _acknowledgements[node_id(ack.host_id, ack.service_id)] = ack;
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = *std::static_pointer_cast<neb::downtime const>(d);
    logging::debug(logging::medium)
      << "node events: loading downtime for ("
      << dwn.host_id << ", " << dwn.service_id << ")"
      << ", starting at " << dwn.start_time;
    _register_downtime(*std::static_pointer_cast<downtime const>(d), NULL);
  }
}

/**
 *  Apply the downtimes configured.
 */
void node_events_stream::_apply_config_downtimes() {
  // Working set of found downtimes.
  std::set<unsigned int> found_downtime_ids;
  // Publisher.
  multiplexing::publisher pblsh;

  // For each downtimes loaded from the config.
  for (std::vector<neb::downtime>::iterator
         it = _incomplete_downtime.begin(),
         end = _incomplete_downtime.end();
       it != end;
       ++it) {
    // Try to find a matching loaded downtime.
    bool found_matching_downtime = false;
    bool is_recurring = !it->recurring_timeperiod.empty();
    node_id id(it->host_id, it->service_id);

    std::list<downtime> downtimes = !is_recurring
      ? _downtimes.get_all_downtimes_of_node(id)
      : _downtimes.get_all_recurring_downtimes_of_node(id);

    for (std::list<downtime>::const_iterator it_set{downtimes.begin()},
         end_set{downtimes.end()};
         it_set != end_set; ++it_set)
      if (it_set->start_time == it->start_time &&
          it_set->end_time == it->end_time && it_set->come_from == 1 &&
          it_set->recurring_timeperiod == it->recurring_timeperiod) {
        found_downtime_ids.insert(it_set->internal_id);
        found_matching_downtime = true;
        break;
      }

    // No matching loaded downtime found, create one.
    if (!found_matching_downtime) {
      it->internal_id = _downtimes.get_new_downtime_id();
      it->downtime_type = it->service_id != 0
        ? down_service : down_host;
      it->entry_time = ::time(NULL);
      found_downtime_ids.insert(it->internal_id);
      _register_downtime(*it, &pblsh);
    }
  }

  // Saved downtimes coming from configuration that wasn't in the config file
  // have been deleted.
  std::list<downtime> downtimes = _downtimes.get_all_downtimes();
  for (std::list<downtime>::const_iterator
         it = downtimes.begin(),
         end = downtimes.end();
       it != end;
       ++it)
    if (found_downtime_ids.find(it->internal_id) == found_downtime_ids.end())
      _delete_downtime(*it, ::time(NULL), &pblsh);
}

/**
 *  Save the cache.
 */
void node_events_stream::_save_cache() {
  // No cache, nothing to do.
  if (_cache.get() == NULL)
    return ;

  logging::info(logging::medium) << "node events: saving cache";

  _cache->transaction();
  // Serialize the node cache.
  _node_cache.serialize(_cache);
  // Managed internally.
  for (std::unordered_map<node_id, neb::acknowledgement>::const_iterator
         it = _acknowledgements.begin(),
         end = _acknowledgements.end();
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::acknowledgement>(it->second));
  std::list<downtime> downtimes = _downtimes.get_all_downtimes();
  for (std::list<downtime>::const_iterator
         it = downtimes.begin(),
         end = downtimes.end();
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::downtime>(*it));
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
    time_t now(::time(NULL));
    if (id.is_host()) {
      neb::host_status* hst = _node_cache.get_host_status(id);
      if (hst != NULL
            && hst->last_hard_state != 0
            && now >= dwn.start_time
            && now < dwn.end_time)
        _downtime_scheduler.add_downtime(
                              now,
                              now + dwn.duration,
                              dwn);
    }
    else {
      neb::service_status* sst = _node_cache.get_service_status(id);
      if (sst != NULL
          && sst->last_hard_state != 0
          && now >= dwn.start_time
          && now < dwn.end_time)
        _downtime_scheduler.add_downtime(
                              now,
                              now + dwn.duration,
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
  if (_downtimes.spawned_downtime_exist(dwn.internal_id))
    return ;

  // Spawn a new downtime.
  downtime spawned(dwn);
  spawned.triggered_by = dwn.internal_id;
  spawned.is_recurring = false;
  spawned.internal_id = _downtimes.get_new_downtime_id();

  // Get the timeperiod.
  std::unordered_map<std::string, time::timeperiod::ptr>::const_iterator tp{
      _timeperiods.find(dwn.recurring_timeperiod)};

  if (tp == _timeperiods.end()) {
    logging::error(logging::medium)
      << "node events: ignoring recurring downtime "
      << dwn.internal_id << ", timeperiod '" << dwn.recurring_timeperiod
      << "' does not exist";
    return ;
  }

  // Get the new start and end time.
  if (when.is_null())
    when = ::time(NULL);
  // Downtime expired, my friend.
  if (when >= dwn.end_time) {
    _delete_downtime(dwn, ::time(NULL), NULL);
    return ;
  }

  spawned.start_time = tp->second->get_next_valid(dwn.start_time < when ? when : dwn.start_time);
  spawned.end_time = tp->second->get_next_invalid(spawned.start_time);
  if (spawned.end_time > dwn.end_time)
    spawned.end_time = dwn.end_time;
  spawned.entry_time = ::time(NULL);

  // Save the downtime.
  _downtimes.add_downtime(spawned);

  // Send the downtime.
  multiplexing::publisher pblsh;
  pblsh.write(std::make_shared<neb::downtime>(spawned));

  // Schedule the downtime.
  _schedule_downtime(spawned);
}
