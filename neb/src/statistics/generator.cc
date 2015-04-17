/*
** Copyright 2013-2015 Merethis
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

#include <QDomDocument>
#include <QDomElement>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/neb/statistics/active_host_execution_time.hh"
#include "com/centreon/broker/neb/statistics/active_host_latency.hh"
#include "com/centreon/broker/neb/statistics/active_host_state_change.hh"
#include "com/centreon/broker/neb/statistics/active_hosts_last.hh"
#include "com/centreon/broker/neb/statistics/active_service_execution_time.hh"
#include "com/centreon/broker/neb/statistics/active_service_latency.hh"
#include "com/centreon/broker/neb/statistics/active_service_state_change.hh"
#include "com/centreon/broker/neb/statistics/active_services_last.hh"
#include "com/centreon/broker/neb/statistics/command_buffers.hh"
#include "com/centreon/broker/neb/statistics/generator.hh"
#include "com/centreon/broker/neb/statistics/hosts.hh"
#include "com/centreon/broker/neb/statistics/hosts_actively_checked.hh"
#include "com/centreon/broker/neb/statistics/hosts_checked.hh"
#include "com/centreon/broker/neb/statistics/hosts_flapping.hh"
#include "com/centreon/broker/neb/statistics/hosts_scheduled.hh"
#include "com/centreon/broker/neb/statistics/passive_host_latency.hh"
#include "com/centreon/broker/neb/statistics/passive_host_state_change.hh"
#include "com/centreon/broker/neb/statistics/passive_hosts_last.hh"
#include "com/centreon/broker/neb/statistics/passive_service_latency.hh"
#include "com/centreon/broker/neb/statistics/passive_service_state_change.hh"
#include "com/centreon/broker/neb/statistics/passive_services_last.hh"
#include "com/centreon/broker/neb/statistics/services.hh"
#include "com/centreon/broker/neb/statistics/services_actively_checked.hh"
#include "com/centreon/broker/neb/statistics/services_checked.hh"
#include "com/centreon/broker/neb/statistics/services_flapping.hh"
#include "com/centreon/broker/neb/statistics/services_scheduled.hh"
#include "com/centreon/broker/neb/statistics/total_host_state_change.hh"
#include "com/centreon/broker/neb/statistics/total_hosts.hh"
#include "com/centreon/broker/neb/statistics/total_service_state_change.hh"
#include "com/centreon/broker/neb/statistics/total_services.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
generator::generator()
  : _interval(0) {
  _plugins["active_host_execution_time"] = misc::shared_ptr<plugin>(new active_host_execution_time);
  _plugins["active_host_latency"] = misc::shared_ptr<plugin>(new active_host_latency);
  _plugins["active_hosts_last"] = misc::shared_ptr<plugin>(new active_hosts_last);
  _plugins["active_host_state_change"] = misc::shared_ptr<plugin>(new active_host_state_change);
  _plugins["active_service_execution_time"] = misc::shared_ptr<plugin>(new active_service_execution_time);
  _plugins["active_service_latency"] = misc::shared_ptr<plugin>(new active_service_latency);
  _plugins["active_services_last"] = misc::shared_ptr<plugin>(new active_services_last);
  _plugins["active_service_state_change"] = misc::shared_ptr<plugin>(new active_service_state_change);
  _plugins["command_buffers"] = misc::shared_ptr<plugin>(new command_buffers);
  _plugins["hosts_actively_checked"] = misc::shared_ptr<plugin>(new hosts_actively_checked);
  _plugins["hosts_checked"] = misc::shared_ptr<plugin>(new hosts_checked);
  _plugins["hosts_flapping"] = misc::shared_ptr<plugin>(new hosts_flapping);
  _plugins["hosts"] = misc::shared_ptr<plugin>(new hosts);
  _plugins["hosts_scheduled"] = misc::shared_ptr<plugin>(new hosts_scheduled);
  _plugins["passive_host_latency"] = misc::shared_ptr<plugin>(new passive_host_latency);
  _plugins["passive_hosts_last"] = misc::shared_ptr<plugin>(new passive_hosts_last);
  _plugins["passive_host_state_change"] = misc::shared_ptr<plugin>(new passive_host_state_change);
  _plugins["passive_service_latency"] = misc::shared_ptr<plugin>(new passive_service_latency);
  _plugins["passive_services_last"] = misc::shared_ptr<plugin>(new passive_services_last);
  _plugins["passive_service_state_change"] = misc::shared_ptr<plugin>(new passive_service_state_change);
  _plugins["services_actively_checked"] = misc::shared_ptr<plugin>(new services_actively_checked);
  _plugins["services_checked"] = misc::shared_ptr<plugin>(new services_checked);
  _plugins["services_flapping"] = misc::shared_ptr<plugin>(new services_flapping);
  _plugins["services"] = misc::shared_ptr<plugin>(new services);
  _plugins["services_scheduled"] = misc::shared_ptr<plugin>(new services_scheduled);
  _plugins["total_hosts"] = misc::shared_ptr<plugin>(new total_hosts);
  _plugins["total_host_state_change"] = misc::shared_ptr<plugin>(new total_host_state_change);
  _plugins["total_services"] = misc::shared_ptr<plugin>(new total_services);
  _plugins["total_service_state_change"] = misc::shared_ptr<plugin>(new total_service_state_change);
}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
generator::generator(generator const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
generator::~generator() {

}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
generator& generator::operator=(generator const& right) {
  if (this != &right) {
    _interval = right._interval;
    _registers = right._registers;
  }
  return (*this);
}

/**
 *  Add plugin to execute.
 *
 *  @param[in] host_id    The host id.
 *  @param[in] service_id The service id.
 *  @param[in] plugin     The plugin to add.
 */
void generator::add(
                  unsigned int host_id,
                  unsigned int service_id,
                  misc::shared_ptr<plugin> plugin) {
  if (!host_id)
    throw (exceptions::msg() << "stats: invalid plugin host id");
  if (!service_id)
    throw (exceptions::msg() << "stats: invalid plugin service id");

  std::pair<unsigned int, unsigned int>
    ids(std::make_pair(host_id, service_id));
  _registers.insert(std::make_pair(ids, plugin));
  return ;
}

/**
 *  Add plugin to execute.
 *
 *  @param[in] host_id    The host id.
 *  @param[in] service_id The service id.
 *  @param[in] name       The plugin name.
 */
void generator::add(
                  unsigned int host_id,
                  unsigned int service_id,
                  std::string const& name) {
  std::map<std::string, misc::shared_ptr<plugin> >::const_iterator
    it(_plugins.find(name));
  if (it == _plugins.end())
    throw (exceptions::msg() << "stats: invalid plugin name");
  add(host_id, service_id, it->second);
  return ;
}

/**
 *  Clear plugins list.
 */
void generator::clear() {
  _registers.clear();
  return ;
}

/**
 *  Get the statistics interval.
 *
 *  @return The interval to generate statistics.
 */
unsigned int generator::interval() const throw () {
  return (_interval);
}

/**
 *  Set the statistics interval.
 *
 *  @param[in] interval The interval to generate statistics.
 */
void generator::interval(unsigned int value) {
  _interval = value;
}

/**
 *  Remove plugin.
 *
 *  @param[in] host_id    The host id.
 *  @param[in] service_id The service id.
 */
void generator::remove(
                  unsigned int host_id,
                  unsigned int service_id) {
  std::map<
         std::pair<unsigned int, unsigned int>,
         misc::shared_ptr<plugin> >::iterator
    it(_registers.find(std::make_pair(host_id, service_id)));
  if (it != _registers.end())
    _registers.erase(it);
  return ;
}

/**
 *  Execute all plugins.
 */
void generator::run() {
  time_t now(time(NULL));
  for (std::map<std::pair<unsigned int, unsigned int>, misc::shared_ptr<plugin> >::const_iterator
         it(_registers.begin()), end(_registers.end());
       it != end;
       ++it) {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->check_interval = _interval;
    ss->last_check = now;
    ss->last_update = now;
    ss->host_id = it->first.first;
    ss->service_id = it->first.second;
    ss->host_name = instance_name;
    ss->service_description = it->second->name().c_str();
    try {
      std::string output;
      std::string perfdata;
      it->second->run(output, perfdata);
      ss->output = output.c_str();
      ss->perf_data = perfdata.c_str();
    }
    catch (std::exception const& e) {
      ss->output = e.what();
    }

    // Send event.
    gl_publisher.write(ss);
  }
  return ;
}

/**
 *  Set configuration.
 *
 *  @param[in] config The configuration to set.
 */
void generator::set(config::state const& config) {
  QMap<QString, QString>::const_iterator
    it(config.params().find("stats"));
  if (it == config.params().end())
    return ;

  // Parse XML.
  QDomDocument d;
  if (d.setContent(it.value())) {
    QDomElement root(d.documentElement());

    QDomElement remote(root.lastChildElement("remote"));
    if (!remote.isNull()) {
      QDomElement interval(remote.lastChildElement("interval"));
      if (!interval.isNull())
        _interval = interval.text().toUInt();

      QDomElement metrics(remote.lastChildElement("metrics"));
      if (!metrics.isNull()) {
        QDomElement host(metrics.lastChildElement("host"));
        if (host.isNull())
          throw (exceptions::msg() << "stats: invalid remote host");
        unsigned int host_id(host.text().toUInt());

        QDomElement service(metrics.firstChildElement("service"));
        while (!service.isNull()) {
          QDomElement id(service.firstChildElement("id"));
          if (id.isNull())
            throw (exceptions::msg()
                   << "stats: invalid remote service id");
          QDomElement name(service.firstChildElement("name"));
          if (name.isNull())
            throw (exceptions::msg()
                   << "stats: invalid remote service name");

          logging::config(logging::medium)
            << "stats: new service (host " << host_id << ", service "
            << id.text().toUInt() << ", name " << name.text() << ")";
          add(host_id, id.text().toUInt(), name.text().toStdString());

          service = service.nextSiblingElement("service");
        }
      }
    }
  }

  return ;
}
