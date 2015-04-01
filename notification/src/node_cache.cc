/*
** Copyright 2014 Merethis
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
#include <vector>
#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include <exception>
#include <QMutexLocker>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/acknowledgement_removed.hh"
#include "com/centreon/broker/notification/downtime_removed.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**
 *  Constructor.
 *
 *  @param[in] cache  The persistent cache used by the node cache.
 */
node_cache::node_cache(misc::shared_ptr<persistent_cache> cache)
  : _mutex(QMutex::NonRecursive),
    _cache(cache),
    _actual_downtime_id(0) {
  multiplexing::engine::instance().hook(*this);
}

/**
 *  Destructor.
 */
node_cache::~node_cache() {
  multiplexing::engine::instance().unhook(*this);
}


/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
node_cache::node_cache(node_cache const& obj) {
  node_cache::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to his object.
 */
node_cache& node_cache::operator=(node_cache const& obj) {
  if (this != &obj) {
    _host_node_states = obj._host_node_states;
    _service_node_states = obj._service_node_states;
    _cache = obj._cache;
    _actual_downtime_id = obj._actual_downtime_id;
  }
  return (*this);
}

/**
 *  Called when the engine starts. Used to load cache file.
 */
void node_cache::starting() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  logging::debug(logging::low)
    << "notification: loading the node cache " << _cache->get_cache_file();

  misc::shared_ptr<io::data> data;
  try {
    while (true) {
      _cache->get(data);
      if (data.isNull())
        break ;
      write(data);
      write_downtime_or_ack(data);
    }
  }
  catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "notification: could not load the node cache "
      << _cache->get_cache_file() << ": " << e.what();
    return ;
  }

  logging::debug(logging::low)
    << "notification: finished loading the node cache "
    << _cache->get_cache_file() << " succesfully";
}

/**
 *  Called when the engine stops. Used to unload cache file.
 */
void node_cache::stopping() {
  // No cache, nothing to do.
  if (_cache.isNull())
    return ;

  logging::debug(logging::low)
    << "notification: writing the node cache " << _cache->get_cache_file();

  // Start a transaction.
  _cache->transaction();

  // Lock the mutex;
  QMutexLocker lock(&_mutex);

  // Prepare serialization.
  _prepare_serialization();

  misc::shared_ptr<io::data> data;

  try {
    while (true) {
      read(data);
      _cache->add(data);
    }
  }
  catch (io::exceptions::shutdown const& s) {
    // Normal termination of the stream (ie nothing to write anymore).
    (void)s;
    logging::debug(logging::low)
      << "notification: finished writing the node cache "
      << _cache->get_cache_file() << " succesfully";
  }
  catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "notification: could not write the node cache "
      << _cache->get_cache_file() << ": " << e.what();
    return ;
  }

  logging::debug(logging::low)
    << "notification: commiting the node cache '"
    << _cache->get_cache_file() << "'";

  try {
    _cache->commit();
  } catch (std::exception const& e) {
    logging::error(logging::high)
      << "notification: could not commit the node cache '"
      << _cache->get_cache_file() << "': " << e.what();
  }

  logging::debug(logging::low)
    << "notification: commited the node cache '"
    << _cache->get_cache_file() << "' succesfully";
}

/**
 *  Read events from the node cache.
 *
 *  @param[out] d  An output data event.
 */
void node_cache::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  if (!_serialized_data.empty()) {
    d = _serialized_data.front();
    _serialized_data.pop_front();
  }
}

/**
 *  Write event to the node cache.
 *
 *  @param[in] data  The data event.
 *
 *  @return          Number of event acknowledged.
 */
unsigned int node_cache::write(misc::shared_ptr<io::data> const& data) {
  // Check that data exists.
  if (data.isNull())
    return (1);

  unsigned int type = data->type();
  if (type == neb::host::static_type())
    update(*data.staticCast<neb::host>());
  else if (type == neb::host_status::static_type())
    update(*data.staticCast<neb::host_status>());
  else if (type == neb::host_group_member::static_type())
    update(*data.staticCast<neb::host_group_member>());
  else if (type == neb::service::static_type())
    update(*data.staticCast<neb::service>());
  else if (type == neb::service_status::static_type())
    update(*data.staticCast<neb::service_status>());
  else if (type == neb::service_group_member::static_type())
    update(*data.staticCast<neb::service_group_member>());
  else if (type == neb::custom_variable::static_type()
           || type == neb::custom_variable_status::static_type())
    update(*data.staticCast<neb::custom_variable_status>());
  else if (type == command_file::external_command::static_type()) {
    try {
      _serialized_data.push_back(
        parse_command(data.ref_as<command_file::external_command const>()));
    } catch (std::exception const& e) {
      logging::error(logging::medium)
        << "notification: can't parse command '"
        << data.ref_as<command_file::external_command>().command
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
unsigned int node_cache::write_downtime_or_ack(
                            misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return (1);

  if (d->type() == neb::downtime::static_type()) {
    neb::downtime const& down = d.ref_as<neb::downtime const>();
    _downtimes[down.internal_id] = down;
    _downtime_id_by_nodes.insert(
      objects::node_id(down.host_id, down.service_id),
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
}

/**
 *  Update the node cache.
 *
 *  @param[in] hst  The data to update.
 */
void node_cache::update(neb::host const& hst) {
  if (hst.host_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hst.host_id)].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hst  The data to update.
 */
void node_cache::update(neb::host_status const& hst) {
  if (hst.host_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hst.host_id)].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hgm  The data to update.
 */
void node_cache::update(neb::host_group_member const& hgm) {
  if (hgm.host_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hgm.host_id)].update(hgm);
}

/**
 *  Update the node cache.
 *
 *  @param[in] s  The data to update.
 */
void node_cache::update(neb::service const& s) {
  if (s.service_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(s.host_id, s.service_id)].update(s);
}

/**
 *  Update the node cache.
 *
 *  @param[in] sst  The data to update.
 */
void node_cache::update(neb::service_status const& sst) {
  if (sst.service_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(sst.host_id, sst.service_id)].update(sst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] sgm  The data to update.
 */
void node_cache::update(neb::service_group_member const& sgm) {
  if (sgm.service_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(sgm.host_id, sgm.service_id)].update(sgm);
}

/**
 *  Update the node cache.
 *
 *  @param[in] cvs  The data to update.
 */
void node_cache::update(neb::custom_variable_status const& cvs) {
  if (cvs.host_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  if (cvs.service_id == 0)
    _host_node_states[objects::node_id(cvs.host_id)].update(cvs);
  else
    _service_node_states[objects::node_id(cvs.host_id, cvs.service_id)].update(cvs);
}

/**
 *  Parse an external command.
 *
 *  @param[in] exc  External command.
 *
 *  @return         An event.
 */
misc::shared_ptr<io::data>
  node_cache::parse_command(command_file::external_command const& exc) {
  std::string line = exc.command.toStdString();
  std::string command;
  std::string args;
  command.resize(line.size());
  args.resize(line.size());

  // Parse timestamp.
  unsigned long timestamp;
  if (::sscanf(
        line.c_str(),
        "[%lu] %[^ ;];%s",
        &timestamp,
        &command[0],
        &args[0]) != 3)
    throw (exceptions::msg()
           << "couldn't parse the line");

  if (command == "ACKNOWLEDGE_HOST_PROBLEM")
    return (_parse_ack(ack_host, timestamp, args));
  else if (command == "ACKNOWLEDGE_SERVICE_PROBLEM")
    return (_parse_ack(ack_service, timestamp, args));
  else if (command == "REMOVE_HOST_ACKNOWLEDGEMENT")
    return (_parse_remove_ack(ack_host, args));
  else if (command == "REMOVE_SVC_ACKNOWLEDGEMENT")
    return (_parse_remove_ack(ack_service, args));
  else if (command == "SCHEDULE_HOST_DOWNTIME")
    return (_parse_downtime(down_host, timestamp, args));
  else if (command == "SCHEDULE_HOST_SVC_DOWNTIME")
    return (_parse_downtime(down_host_service, timestamp, args));
  else if (command == "SCHEDULE_SVC_DOWNTIME")
    return (_parse_downtime(down_host_service, timestamp, args));
  else if (command == "DELETE_HOST_DOWNTIME")
    return (_parse_remove_downtime(down_host, args));
  else if (command == "DELETE_SVC_DOWNTIME")
    return (_parse_remove_downtime(down_service, args));

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
objects::node_id node_cache::get_node_by_names(
                   std::string const& host_name,
                   std::string const& service_description) {
  // TODO: Faster node lookup.

  if (service_description.empty()) {
    for (QHash<objects::node_id, host_node_state>::const_iterator
           it = _host_node_states.begin(),
           end = _host_node_states.end();
         it != end;
         ++it)
      if (it->get_node().host_name == host_name.c_str())
        return (objects::node_id(it->get_node().host_id));
  }
  else {
    for (QHash<objects::node_id, service_node_state>::const_iterator
           it = _service_node_states.begin(),
           end = _service_node_states.end();
         it != end;
         ++it)
      if (it->get_node().host_name == host_name.c_str()
            && it->get_node().service_description == service_description.c_str())
        return (objects::node_id(
                  it->get_node().host_id, it->get_node().service_id));
  }

  return (objects::node_id());
}

/**
 *  Get a host from the node cache.
 *
 *  @param[in] id  The id of the host.
 *
 *  @return        The host from the node cache.
 */
node_cache::host_node_state const& node_cache::get_host(
                                                 objects::node_id id) const {
  objects::node_id host_node_id(id.get_host_id());
  QHash<objects::node_id, host_node_state>::const_iterator found =
    _host_node_states.find(host_node_id);
  if (found == _host_node_states.end())
    throw (exceptions::msg() << "notification: host "
           << id.get_host_id() << " was not found in cache");
  return (*found);
}

/**
 *  Get a service from the service cache.
 *
 *  @param[in] id  The id of the service.
 *
 *  @return        The service from the node cache.
 */
node_cache::service_node_state const& node_cache::get_service(
                                                    objects::node_id id) const {
  QHash<objects::node_id, service_node_state>::const_iterator found =
    _service_node_states.find(id);
  if (found == _service_node_states.end())
    throw (exceptions::msg()
           << "notification: service (" << id.get_host_id() << ", "
           << id.get_service_id() << " was not found in cache");
  return (*found);
}

/**
 *  Get all the node contained in a group.
 *
 *  @param[in] group_name     The name of the group.
 *  @param[in] is_host_group  Is this a host group or a service group?
 *
 *  @return  A list of the name of the node contained in a group.
 */
std::vector<std::string> node_cache::get_all_node_contained_in(
                                       std::string const& group_name,
                                       bool is_host_group) const {
  std::vector<std::string> res;

  if (is_host_group) {
    for (QHash<objects::node_id, host_node_state>::const_iterator
           it(_host_node_states.begin()),
           end(_host_node_states.end());
        it != end;
        ++it)
      if (it->get_groups().count(group_name) != 0)
        res.push_back(it->get_node().host_name.toStdString());
  }
  else {
    for (QHash<objects::node_id, service_node_state>::const_iterator
           it(_service_node_states.begin()),
           end(_service_node_states.end());
        it != end;
        ++it)
      if (it->get_groups().count(group_name) != 0)
        res.push_back(it->get_node().service_description.toStdString());
  }

  return (res);
}

/**
 *  Is this node in downtime ?
 *
 *  @param[in] node  The node.
 *
 *  @return          True if this node is in downtime.
 */
bool node_cache::node_in_downtime(objects::node_id node) const {
  return (_downtime_id_by_nodes.contains(node));
}

/**
 *  Is this node acknowledged ?
 *
 *  @param[in] node  The node.
 *
 *  @return          True if this node was acknowledged.
 */
bool node_cache::node_acknowledged(objects::node_id node) const {
  return (_acknowledgements.contains(node));
}

/**
 *  Prepare the serialization of all the data.
 */
void node_cache::_prepare_serialization() {
  _serialized_data.clear();
  for (QHash<objects::node_id, host_node_state>::const_iterator
         it = _host_node_states.begin(),
         end = _host_node_states.end();
       it != end;
       ++it)
    it->serialize(_serialized_data);
  for (QHash<objects::node_id, service_node_state>::const_iterator
         it = _service_node_states.begin(),
         end = _service_node_states.end();
       it != end;
       ++it)
    it->serialize(_serialized_data);
  for (QHash<objects::node_id, neb::acknowledgement>::const_iterator
         it = _acknowledgements.begin(),
         end = _acknowledgements.end();
       it != end;
       ++it)
    _serialized_data.push_back(
      misc::make_shared(new neb::acknowledgement(*it)));
  for (QHash<unsigned int, neb::downtime>::const_iterator
         it = _downtimes.begin(),
         end = _downtimes.end();
       it != end;
       ++it)
    _serialized_data.push_back(misc::make_shared(new neb::downtime(*it)));
}

/**
 *  Parse an acknowledgment.
 *
 *  @param[in] is_host  Is this a host acknowledgement.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *
 *  @return             An acknowledgement event.
 */
misc::shared_ptr<io::data> node_cache::_parse_ack(
                             ack_type is_host,
                             timestamp t,
                             std::string const& args) {
  std::string host_name;
  std::string service_description;
  int sticky = 0;
  int notify = 0;
  int persistent_comment = 0;
  std::string author;
  std::string comment;
  host_name.resize(args.size());
  service_description.resize(args.size());
  author.resize(args.size());
  comment.resize(args.size());
  bool ret = false;
  if (is_host == ack_host)
    ret = (::sscanf(
             args.c_str(),
             "%[^;];%i;%i;%i;%[^;];%[^;]",
             &host_name[0],
             &sticky,
             &notify,
             &persistent_comment,
             &author[0],
             &comment[0]) == 6);
  else
    ret = (::sscanf(
             args.c_str(),
             "%[^;];%[^;];%i;%i;%i;%[^;];%[^;]",
             &host_name[0],
             &service_description[0],
             &sticky,
             &notify,
             &persistent_comment,
             &author[0],
             &comment[0]) == 7);
  if (!ret)
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement");

  objects::node_id id = get_node_by_names(host_name, service_description);
  misc::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);
  ack->acknowledgement_type = is_host;
  ack->comment = QString::fromStdString(comment);
  ack->author = QString::fromStdString(author);
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
 *  @param[in] args     The args to parse.
 *
 *  @return             An acknowledgement removal event.
 */
misc::shared_ptr<io::data> node_cache::_parse_remove_ack(
                             ack_type type,
                             std::string const& args) {
  std::string host_name;
  std::string service_description;
  host_name.resize(args.size());
  service_description.resize(args.size());
  bool ret = false;
  if (type == ack_host)
    ret = (::sscanf(args.c_str(), "%[^;]", &host_name[0]) == 1);
  else
    ret = (::sscanf(
             args.c_str(),
             "%[^;];%[^;]",
             &host_name[0],
             &service_description[0]) == 2);
  if (!ret)
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement removal");

  // Find the node id from the host name / description.
  objects::node_id id = get_node_by_names(host_name, service_description);

  // Find the ack.
  QHash<objects::node_id, neb::acknowledgement>::iterator
    found(_acknowledgements.find(id));
  if (found == _acknowledgements.end())
    throw (exceptions::msg()
           << "couldn't find an acknowledgement for ("
           << id.get_host_id() << ", " << id.get_service_id() << ")");

  // Erase the ack.
  _acknowledgements.erase(found);

  // Send an ack removed event.
  misc::shared_ptr<acknowledgement_removed> a(new acknowledgement_removed);
  a->host_id = id.get_host_id();
  a->service_id = id.get_service_id();
  return (a);
}

/**
 *  Parse a downtime.
 *
 *  @param[in] type     The downtime type.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *
 *  @return             A downtime event.
 */
misc::shared_ptr<io::data>
  node_cache::_parse_downtime(
                down_type type,
                timestamp t,
                std::string const& args) {
  std::string host_name;
  std::string service_description;
  unsigned long start_time = 0;
  unsigned long end_time = 0;
  int fixed = 0;
  unsigned int trigger_id = 0;
  unsigned int duration = 0;
  std::string author;
  std::string comment;
  host_name.resize(args.size());
  service_description.resize(args.size());
  author.resize(args.size());
  comment.resize(args.size());
  bool ret = false;

  if (type == down_host)
    ret = (::sscanf(
             args.c_str(),
             "%[^;];%lu;%lu;%i;%u;%u;%[^;];%[^;]",
             &host_name[0],
             &start_time,
             &end_time,
             &fixed,
             &trigger_id,
             &duration,
             &author[0],
             &comment[0]) == 8);
  else
    ret = (::sscanf(
             args.c_str(),
             "%[^;];%[^;];%lu;%lu;%i;%u;%u;%[^;];%[^;]",
             &host_name[0],
             &service_description[0],
             &start_time,
             &end_time,
             &fixed,
             &trigger_id,
             &duration,
             &author[0],
             &comment[0]) == 9);

  if (!ret)
    throw (exceptions::msg() << "error while parsing downtime arguments");

  objects::node_id id = get_node_by_names(host_name, service_description);

  misc::shared_ptr<neb::downtime> d(new neb::downtime);
  d->author = QString::fromStdString(author);
  d->comment = QString::fromStdString(comment);
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
 *  @param[in] args     The args to parse.
 *
 *  @return             A downtime removal event.
 */
misc::shared_ptr<io::data> node_cache::_parse_remove_downtime(
                             down_type type,
                             std::string const& args) {
  (void)type;
  unsigned int downtime_id;
  if (::sscanf(args.c_str(), "%u", &downtime_id) != 1)
    throw (exceptions::msg() << "error while parsing remove downtime arguments");

  // Find the downtime.
  QHash<unsigned int, neb::downtime>::iterator
    found(_downtimes.find(downtime_id));
  if (found == _downtimes.end())
    throw (exceptions::msg()
           << "couldn't find a downtime for downtime id " << downtime_id);

  objects::node_id node(found->host_id, found->service_id);

  // Erase the downtime.
  _downtimes.erase(found);
  QMultiHash<objects::node_id, unsigned int>::iterator tmp;
  for (QMultiHash<objects::node_id, unsigned int>::iterator
         it = _downtime_id_by_nodes.find(node),
         end = _downtime_id_by_nodes.end();
       it != end && it.key() == node;
       it = tmp) {
    tmp = it;
    ++tmp;
    if (*it == downtime_id)
      _downtime_id_by_nodes.erase(it);
  }


  // Send a downtime removed event.
  misc::shared_ptr<downtime_removed> d(new downtime_removed);
  d->downtime_id = downtime_id;

  return (d);
}
