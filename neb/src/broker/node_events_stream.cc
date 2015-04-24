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
#include "com/centreon/broker/neb/node_id.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/command_file/external_command.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Constructor.
 *
 *  @param[int,out] cache             Persistent cache.
 */
node_events_stream::node_events_stream(
    misc::shared_ptr<persistent_cache> cache)
  : _cache(cache),
    _process_out(true),
    _actual_downtime_id(0) {
  // Load the cache.
  _load_cache();
}

/**
 *  Destructor.
 */
node_events_stream::~node_events_stream() {
  try {
    _save_cache();
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
 *  @param[out] d  Unused.
 */
void node_events_stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
   << "neb: cannot read from a node events stream. This is likely a "
   << "software bug that you should report to Centreon Broker "
   << "developers");
  return ;
}

/**
 *  Update the stream.
 */
void node_events_stream::update() {
  _save_cache();
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

  if (d->type() == neb::host::static_type()) {
    _process_host(d.ref_as<neb::host const>());
  }
  else if (d->type() == neb::service::static_type()) {
    _process_service(d.ref_as<neb::service const>());
  }
  else if (d->type() == command_file::external_command::static_type()) {
    try {
      misc::shared_ptr<io::data> d =
        parse_command(d.ref_as<command_file::external_command const>());
    } catch (std::exception const& e) {
      logging::error(logging::medium)
        << "neb: node events stream can't parse command '"
        << d.ref_as<command_file::external_command>().command
        << "': " << e.what();
    }
  }
  return (1);
}

/**
 *  Write a downtime or an ack to the node cache.
 *
 *  @param[in] d  Downtime or ack.
 *
 *  @return       The number of events acknowledged.
 */
/*unsigned int node_events_stream::write_downtime_or_ack(
                            misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return (1);

  if (d->type() == neb::downtime::static_type()) {
    neb::downtime const&
      down = d.ref_as<neb::downtime const>();
    _downtimes[down.internal_id] = down;
    _downtime_id_by_nodes.insert(
      node_id(down.host_id, down.service_id),
      down.internal_id);
    if (_actual_downtime_id <= down.internal_id)
      _actual_downtime_id = down.internal_id + 1;
  }
  else if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack = d.ref_as<neb::acknowledgement const>();
    _acknowledgements[objects::node_id(ack.host_id, ack.service_id)]
      = ack;
  }
  return (1);
}*/

/**
 *  This class holds a RAII buffer for parsing.
 */
class buffer {
public:
  explicit buffer(unsigned int size) {
    _data = new char[size];
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
 *  @param[in] exc  External command.
 *
 *  @return         An event.
 */
misc::shared_ptr<io::data>
  node_events_stream::parse_command(command_file::external_command const& exc) {
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

  size_t arg_len = ::strlen(args.get());

  if (command == "ACKNOWLEDGE_HOST_PROBLEM")
    return (_parse_ack(ack_host, timestamp, args.get(), arg_len));
  else if (command == "ACKNOWLEDGE_SVC_PROBLEM")
    return (_parse_ack(ack_service, timestamp, args.get(), arg_len));
  else if (command == "REMOVE_HOST_ACKNOWLEDGEMENT")
    return (_parse_remove_ack(ack_host, timestamp, args.get(), arg_len));
  else if (command == "REMOVE_SVC_ACKNOWLEDGEMENT")
    return (_parse_remove_ack(ack_service, timestamp, args.get(), arg_len));
  else if (command == "SCHEDULE_HOST_DOWNTIME")
    return (_parse_downtime(down_host, timestamp, args.get(), arg_len));
  else if (command == "SCHEDULE_HOST_SVC_DOWNTIME")
    return (_parse_downtime(down_host_service, timestamp, args.get(), arg_len));
  else if (command == "SCHEDULE_SVC_DOWNTIME")
    return (_parse_downtime(down_service, timestamp, args.get(), arg_len));
  else if (command == "DELETE_HOST_DOWNTIME")
    return (_parse_remove_downtime(down_host, timestamp, args.get(), arg_len));
  else if (command == "DELETE_SVC_DOWNTIME")
    return (_parse_remove_downtime(down_service, timestamp, args.get(), arg_len));

  return (misc::shared_ptr<io::data>());
}

/**
 *  Get a node by its names.
 *
 *  @param[in] host_name            The host name.
 *  @param[in] service_description  The service description, or empty.
 *
 *  @return  The node id.
 */
node_id node_events_stream::get_node_by_names(
          std::string const& host_name,
          std::string const& service_description) {
  QHash<QPair<QString, QString>, node_id>::const_iterator
    found(_names_to_node.find(qMakePair(
                                QString::fromStdString(host_name),
                                QString::fromStdString(service_description))));
  if (found != _names_to_node.end())
    return (*found);
  else
    return (node_id());
}

/**
 *  Is this node in downtime ?
 *
 *  @param[in] node  The node.
 *
 *  @return          True if this node is in downtime.
 */
bool node_events_stream::node_in_downtime(node_id node) const {
  return (_downtime_id_by_nodes.contains(node));
}

/**
 *  Is this node acknowledged ?
 *
 *  @param[in] node  The node.
 *
 *  @return          True if this node was acknowledged.
 */
bool node_events_stream::node_acknowledged(node_id node) const {
  return (_acknowledgements.contains(node));
}

/**
 *  Process a host event.
 *
 *  @param[in] hst  The host event.
 */
void node_events_stream::_process_host(
                           neb::host const& hst) {
  _hosts[node_id(hst.host_id)] = hst;
  _names_to_node[qMakePair(hst.host_name, QString())] = node_id(hst.host_id);
}

/**
 *  Process a service event.
 *
 *  @param[in] svc  The service event.
 */
void node_events_stream::_process_service(
                           neb::service const& svc) {
  _services[node_id(svc.host_id, svc.service_id)] = svc;
  _names_to_node[qMakePair(svc.host_name, svc.service_description)]
    = node_id(svc.host_id, svc.service_id);
}

/**
 *  Parse an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[în] arg_size The size of the arg.
 *
 *  @return             An acknowledgement event.
 */
misc::shared_ptr<io::data> node_events_stream::_parse_ack(
                             ack_type is_host,
                             timestamp t,
                             const char* args,
                             size_t arg_size) {
  buffer host_name(arg_size);
  buffer service_description(arg_size);
  int sticky = 0;
  int notify = 0;
  int persistent_comment = 0;
  buffer author(arg_size);
  buffer comment(arg_size);
  bool ret = false;
  if (is_host == ack_host)
    ret = (::sscanf(
             args,
             "%[^;];%i;%i;%i;%[^;];%[^;]",
             host_name.get(),
             &sticky,
             &notify,
             &persistent_comment,
             author.get(),
             comment.get()) == 6);
  else
    ret = (::sscanf(
             args,
             "%[^;];%[^;];%i;%i;%i;%[^;];%[^;]",
             host_name.get(),
             service_description.get(),
             &sticky,
             &notify,
             &persistent_comment,
             author.get(),
             comment.get()) == 7);
  if (!ret)
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement");

  node_id id(get_node_by_names(
               host_name.get(),
               service_description.get()));
  misc::shared_ptr<neb::acknowledgement>
    ack(new neb::acknowledgement);
  ack->acknowledgement_type = is_host;
  ack->comment = QString(comment.get());
  ack->author = QString(author.get());
  ack->entry_time = t;
  ack->host_id = id.get_host_id();
  ack->service_id = id.get_service_id();
  ack->is_sticky = (sticky == 2);
  ack->persistent_comment = (persistent_comment == 1);
  ack->notify_contacts = (notify == 1);

  // Save acknowledgements.
  _acknowledgements[id] = *ack;

  return (ack);
}

/**
 *  Parse the removal of an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[in] arg_size The size of the arg.
 *
 *  @return             An acknowledgement removal event.
 */
misc::shared_ptr<io::data> node_events_stream::_parse_remove_ack(
                             ack_type type,
                             timestamp t,
                             const char* args,
                             size_t arg_size) {
  buffer host_name(arg_size);
  buffer service_description(arg_size);
  bool ret = false;
  if (type == ack_host)
    ret = (::sscanf(args, "%[^;]", host_name.get()) == 1);
  else
    ret = (::sscanf(
             args,
             "%[^;];%[^;]",
             host_name.get(),
             service_description.get()) == 2);
  if (!ret)
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement removal");

  // Find the node id from the host name / description.
  node_id id = get_node_by_names(
                 host_name.get(),
                 service_description.get());

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

  // Return the closed ack.
  return (ack);
}

/**
 *  Parse a downtime.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[în] arg_size The size of the arg.
 *
 *  @return             A downtime event.
 */
misc::shared_ptr<io::data> node_events_stream::_parse_downtime(
                             down_type type,
                             timestamp t,
                             char const* args,
                             size_t arg_size) {
  buffer host_name(arg_size);
  buffer service_description(arg_size);
  unsigned long start_time = 0;
  unsigned long end_time = 0;
  int fixed = 0;
  unsigned int trigger_id = 0;
  unsigned int duration = 0;
  buffer author(arg_size);
  buffer comment(arg_size);
  bool ret = false;

  (void)t;
  logging::debug(logging::medium)
    << "notification: parsing downtime command: '" << args << "'";

  if (type == down_host)
    ret = (::sscanf(
             args,
             "%[^;];%lu;%lu;%i;%u;%u;%[^;];%[^;]",
             host_name.get(),
             &start_time,
             &end_time,
             &fixed,
             &trigger_id,
             &duration,
             author.get(),
             comment.get()) == 8);
  else
    ret = (::sscanf(
             args,
             "%[^;];%[^;];%lu;%lu;%i;%u;%u;%[^;];%[^;]",
             host_name.get(),
             service_description.get(),
             &start_time,
             &end_time,
             &fixed,
             &trigger_id,
             &duration,
             author.get(),
             comment.get()) == 9);

  if (!ret)
    throw (exceptions::msg() << "error while parsing downtime arguments");

  node_id id = get_node_by_names(
                 host_name.get(),
                 service_description.get());

  misc::shared_ptr<neb::downtime>
    d(new neb::downtime);
  d->author = QString::fromStdString(author.get());
  d->comment = QString::fromStdString(comment.get());
  d->start_time = start_time;
  d->end_time = end_time;
  d->duration = duration;
  d->fixed = (fixed == 1);
  d->downtime_type = type;
  d->host_id = id.get_host_id();
  d->service_id = id.get_service_id();
  d->was_started = true;
  d->triggered_by = trigger_id;
  d->internal_id = ++_actual_downtime_id;

  // Save the downtime.
  _downtimes[d->internal_id] = *d;
  _downtime_id_by_nodes.insert(id, d->internal_id);

  return (d);
}

/**
 *  Parse a downtime removal.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *  @param[în] arg_size The size of the arg.
 *
 *  @return             A downtime removal event.
 */
misc::shared_ptr<io::data> node_events_stream::_parse_remove_downtime(
                             down_type type,
                             timestamp t,
                             const char* args,
                             size_t arg_size) {
  (void)type;
  (void)arg_size;
  unsigned int downtime_id;
  if (::sscanf(args, "%u", &downtime_id) != 1)
    throw (exceptions::msg() << "error while parsing remove downtime arguments");

  // Find the downtime.
  QHash<unsigned int, neb::downtime>::iterator
    found(_downtimes.find(downtime_id));
  if (found == _downtimes.end())
    throw (exceptions::msg()
           << "couldn't find a downtime for downtime id " << downtime_id);

  node_id node(found->host_id, found->service_id);

  // Close the downtime.
  misc::shared_ptr<neb::downtime> d(new neb::downtime(*found));
  d->actual_end_time = t;
  d->deletion_time = t;
  d->was_cancelled = true;

  // Erase the downtime.
  _downtimes.erase(found);
  QMultiHash<node_id, unsigned int>::iterator tmp;
  for (QMultiHash<node_id, unsigned int>::iterator
         it = _downtime_id_by_nodes.find(node),
         end = _downtime_id_by_nodes.end();
       it != end && it.key() == node;
       it = tmp) {
    tmp = it;
    ++tmp;
    if (*it == downtime_id)
      _downtime_id_by_nodes.erase(it);
  }

  // Return the closed downtime.
  return (d);
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
  if (d->type() == neb::host::static_type())
    _process_host(d.ref_as<neb::host const>());
  else if (d->type() == neb::service::static_type())
    _process_service(d.ref_as<neb::service const>());
  else if (d->type() == neb::acknowledgement::static_type()) {
    neb::acknowledgement const& ack = d.ref_as<neb::acknowledgement const>();
    _acknowledgements[node_id(ack.host_id, ack.service_id)] = ack;
  }
  else if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& dwn = d.ref_as<neb::downtime const>();
    _downtimes[dwn.internal_id] = dwn;
    if (_actual_downtime_id < dwn.internal_id)
      _actual_downtime_id = dwn.internal_id + 1;
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
  for (QHash<node_id, neb::host>::const_iterator
         it = _hosts.begin(),
         end = _hosts.end();
       it != end;
       ++it)
    _cache->add(misc::make_shared(new neb::host(*it)));
  for (QHash<node_id, neb::service>::const_iterator
         it = _services.begin(),
         end = _services.end();
       it != end;
       ++it)
    _cache->add(misc::make_shared(new neb::service(*it)));
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
  _cache->commit();
}
