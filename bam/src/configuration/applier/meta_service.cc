/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/configuration/applier/meta_service.hh"
#include <sstream>
#include "com/centreon/broker/bam/metric_book.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/service.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::meta_service::meta_service() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
applier::meta_service::meta_service(applier::meta_service const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
applier::meta_service::~meta_service() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
applier::meta_service& applier::meta_service::operator=(
    applier::meta_service const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Apply configuration.
 *
 *  @param[in]  my_meta  Meta-services to apply.
 *  @param[out] book     Metric book.
 */
void applier::meta_service::apply(state::meta_services const& my_meta,
                                  metric_book& book) {
  //
  // DIFF
  //

  // Objects to delete are items remaining in the
  // set at the end of the iteration.
  std::map<uint32_t, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  state::meta_services to_create(my_meta);

  // Objects to modify are items found but
  // with mismatching configuration.
  std::list<configuration::meta_service> to_modify;

  // Iterate through configuration.
  for (state::meta_services::iterator it(to_create.begin()),
       end(to_create.end());
       it != end;) {
    std::map<uint32_t, applied>::iterator cfg_it(to_delete.find(it->first));
    // Found = modify (or not).
    if (cfg_it != to_delete.end()) {
      // Configuration mismatch, modify object.
      if (cfg_it->second.cfg != it->second)
        to_modify.push_back(it->second);
      to_delete.erase(cfg_it);
      bam::configuration::state::meta_services::iterator tmp = it;
      ++it;
      to_create.erase(tmp);
    }
    // Not found = create.
    else
      ++it;
  }

  //
  // OBJECT CREATION/DELETION
  //

  // Delete objects.
  for (std::map<uint32_t, applied>::iterator it(to_delete.begin()),
       end(to_delete.end());
       it != end; ++it) {
    logging::config(logging::medium)
        << "BAM: removing meta-service " << it->second.cfg.get_id();
    std::shared_ptr<neb::service> s(
        _meta_service(it->first, it->second.cfg.get_host_id(),
                      it->second.cfg.get_service_id()));
    s->enabled = false;
    book.unlisten(it->second.cfg.get_id(), it->second.obj.get());
    _applied.erase(it->first);
    multiplexing::publisher().write(s);
  }
  to_delete.clear();

  // Create new objects.
  for (state::meta_services::iterator it(to_create.begin()),
       end(to_create.end());
       it != end; ++it) {
    logging::config(logging::medium)
        << "BAM: creating meta-service " << it->first;
    std::shared_ptr<bam::meta_service> new_meta(_new_meta(it->second, book));
    applied& content(_applied[it->first]);
    content.cfg = it->second;
    content.obj = new_meta;
    std::shared_ptr<neb::host> h(_meta_host(it->second.get_host_id()));
    multiplexing::publisher().write(h);
    std::shared_ptr<neb::service> s(_meta_service(
        it->first, it->second.get_host_id(), it->second.get_service_id()));
    multiplexing::publisher().write(s);
  }

  // Modify existing objects.
  for (std::list<configuration::meta_service>::iterator it(to_modify.begin()),
       end(to_modify.end());
       it != end; ++it) {
    std::map<uint32_t, applied>::iterator pos(_applied.find(it->get_id()));
    if (pos != _applied.end()) {
      logging::config(logging::medium)
          << "BAM: modifying meta-service " << it->get_id();
      _modify_meta(*pos->second.obj, book, pos->second.cfg, *it);
      pos->second.cfg = *it;
    } else
      logging::error(logging::high)
          << "BAM: attempting to modify meta-service " << it->get_id()
          << ", however associated object was not found. This is likely a"
          << " software bug that you should report to Centreon Broker "
          << "developers";
  }

  return;
}

/**
 *  Find a meta-service by its ID.
 *
 *  @param[in] id  Meta-service ID.
 *
 *  @return Shared pointer to the applied meta-service object.
 */
std::shared_ptr<bam::meta_service> applier::meta_service::find_meta(
    uint32_t id) {
  std::map<uint32_t, applied>::iterator it(_applied.find(id));
  return ((it != _applied.end()) ? it->second.obj
                                 : std::shared_ptr<bam::meta_service>());
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void applier::meta_service::_internal_copy(applier::meta_service const& other) {
  _applied = other._applied;
  return;
}

/**
 *  Get the virtual host of a meta-service.
 *
 *  @param[in] host_id  Host ID.
 *
 *  @return Virtual meta-service host.
 */
std::shared_ptr<neb::host> applier::meta_service::_meta_host(uint32_t host_id) {
  std::shared_ptr<neb::host> h(new neb::host);
  h->host_id = host_id;
  h->host_name = "_Module_Meta";
  h->last_update = time(nullptr);
  h->poller_id =
      com::centreon::broker::config::applier::state::instance().poller_id();
  return (h);
}

/**
 *  Get the virtual service of a meta-service.
 *
 *  @param[in] meta_id     Meta-service ID.
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 *
 *  @return Virtual BA service.
 */
std::shared_ptr<neb::service> applier::meta_service::_meta_service(
    uint32_t meta_id,
    uint32_t host_id,
    uint32_t service_id) {
  std::shared_ptr<neb::service> s(new neb::service);
  s->host_id = host_id;
  s->service_id = service_id;
  {
    std::ostringstream oss;
    oss << "meta_" << meta_id;
    s->service_description = oss.str();
  }
  s->last_update = time(nullptr);
  return (s);
}

/**
 *  Modify a meta-service.
 *
 *  @param[in,out] obj      Meta-service object.
 *  @param[in,out] book     Metric book.
 *  @param[in]     old_cfg  Old configuration.
 *  @param[in]     new_cfg  New configuration.
 */
void applier::meta_service::_modify_meta(
    bam::meta_service& obj,
    metric_book& book,
    configuration::meta_service const& old_cfg,
    configuration::meta_service const& new_cfg) {
  // Remove old metrics from 1) the book and from 2) the meta-service.
  for (configuration::meta_service::metric_container::const_iterator
           it(old_cfg.get_metrics().begin()),
       end(old_cfg.get_metrics().end());
       it != end; ++it) {
    logging::config(logging::low)
        << "BAM: meta-service " << obj.get_id() << " does not depend of metric "
        << *it << " anymore";
    book.unlisten(*it, &obj);
    obj.remove_metric(*it);
  }

  // Add new metrics to 1) the book and to 2) the meta-service.
  for (configuration::meta_service::metric_container::const_iterator
           it(new_cfg.get_metrics().begin()),
       end(new_cfg.get_metrics().end());
       it != end; ++it) {
    logging::config(logging::low)
        << "BAM: meta-service " << obj.get_id() << " uses metric " << *it;
    book.listen(*it, &obj);
    obj.add_metric(*it);
  }

  // Modify meta-service properties.
  std::string const& computation_str(new_cfg.get_computation());
  bam::meta_service::computation_type computation;
  if ("MIN" == computation_str)
    computation = bam::meta_service::min;
  else if ("MAX" == computation_str)
    computation = bam::meta_service::max;
  else if (("SUM" == computation_str) || ("SOM" == computation_str))
    computation = bam::meta_service::sum;
  else
    computation = bam::meta_service::average;
  obj.set_computation(computation);
  obj.set_id(new_cfg.get_id());
  obj.set_host_id(new_cfg.get_host_id());
  obj.set_service_id(new_cfg.get_service_id());
  obj.set_level_warning(new_cfg.get_level_warning());
  obj.set_level_critical(new_cfg.get_level_critical());

  return;
}

/**
 *  Create a new meta-service.
 *
 *  @param[in]  cfg   Meta-service configuration.
 *  @param[out] book  Metric listener book.
 */
std::shared_ptr<bam::meta_service> applier::meta_service::_new_meta(
    configuration::meta_service const& cfg,
    metric_book& book) {
  std::shared_ptr<bam::meta_service> meta(new bam::meta_service);
  _modify_meta(*meta, book, configuration::meta_service(), cfg);
  return (meta);
}
