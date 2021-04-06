/*
** Copyright 2014-2017 Centreon
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

#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include <fmt/format.h>
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
applier::ba::ba() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
applier::ba::ba(applier::ba const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
applier::ba::~ba() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
applier::ba& applier::ba::operator=(applier::ba const& other) {
  if (this != &other)
    _internal_copy(other);
  return *this;
}

/**
 *  Apply configuration.
 *
 *  @param[in] my_bas  BAs to apply.
 *  @param[in] book    The service book.
 */
void applier::ba::apply(bam::configuration::state::bas const& my_bas,
                        service_book& book) {
  //
  // DIFF
  //

  // Objects to delete are items remaining in the
  // set at the end of the iteration.
  std::map<uint32_t, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  bam::configuration::state::bas to_create(my_bas);

  // Objects to modify are items found but
  // with mismatching configuration.
  std::list<bam::configuration::ba> to_modify;

  // Iterate through configuration.
  for (bam::configuration::state::bas::iterator it(to_create.begin()),
       end(to_create.end());
       it != end;) {
    std::map<uint32_t, applied>::iterator cfg_it(to_delete.find(it->first));
    // Found = modify (or not).
    if (cfg_it != to_delete.end()) {
      // Configuration mismatch, modify object.
      if (cfg_it->second.cfg != it->second)
        to_modify.push_back(it->second);
      to_delete.erase(cfg_it);
      bam::configuration::state::bas::iterator tmp = it;
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
    logging::config(logging::medium) << "BAM: removing BA " << it->first;
    std::shared_ptr<neb::service> s(
        _ba_service(it->first, it->second.cfg.get_host_id(),
                    it->second.cfg.get_service_id()));
    s->enabled = false;
    book.unlisten(it->second.cfg.get_host_id(), it->second.cfg.get_service_id(),
                  static_cast<bam::ba*>(it->second.obj.get()));
    _applied.erase(it->first);
    multiplexing::publisher().write(s);
  }
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::bas::iterator it(to_create.begin()),
       end(to_create.end());
       it != end; ++it) {
    logging::config(logging::medium) << "BAM: creating BA " << it->first
                                     << " ('" << it->second.get_name() << "')";
    std::shared_ptr<bam::ba> new_ba(_new_ba(it->second, book));
    applied& content(_applied[it->first]);
    content.cfg = it->second;
    content.obj = new_ba;
    std::shared_ptr<neb::host> h(_ba_host(it->second.get_host_id()));
    multiplexing::publisher().write(h);
    std::shared_ptr<neb::service> s(_ba_service(
        it->first, it->second.get_host_id(), it->second.get_service_id()));
    multiplexing::publisher().write(s);
  }

  // Modify existing objects.
  for (std::list<bam::configuration::ba>::iterator it(to_modify.begin()),
       end(to_modify.end());
       it != end; ++it) {
    std::map<uint32_t, applied>::iterator pos(_applied.find(it->get_id()));
    if (pos != _applied.end()) {
      logging::config(logging::medium) << "BAM: modifying BA " << it->get_id();
      pos->second.obj->set_name(it->get_name());
      pos->second.obj->set_state_source(it->get_state_source());
      pos->second.obj->set_level_warning(it->get_warning_level());
      pos->second.obj->set_level_critical(it->get_critical_level());
      pos->second.cfg = *it;
    } else
      logging::error(logging::high)
          << "BAM: attempting to modify BA " << it->get_id()
          << ", however associated object was not found. This is likely a"
          << " software bug that you should report to Centreon Broker "
          << "developers";
  }

  // Set all BA objects as valid. Invalid BAs will be reset as invalid
  // on KPI application.
  for (std::map<uint32_t, applied>::iterator it(_applied.begin()),
       end(_applied.end());
       it != end; ++it)
    it->second.obj->set_valid(true);
}

/**
 *  Find BA by its ID.
 *
 *  @param[in] id BA ID.
 *
 *  @return Shared pointer to the applied BA object.
 */
std::shared_ptr<bam::ba> applier::ba::find_ba(uint32_t id) {
  std::map<uint32_t, applied>::iterator it(_applied.find(id));
  return (it != _applied.end()) ? it->second.obj : std::shared_ptr<bam::ba>();
}

/**
 *  Visit each applied BA.
 *
 *  @param[out] visitor  Visitor that will receive status.
 */
void applier::ba::visit(io::stream* visitor) {
  for (std::map<uint32_t, applied>::iterator it(_applied.begin()),
       end(_applied.end());
       it != end; ++it)
    it->second.obj->visit(visitor);
}

/**
 *  Get the virtual BA host of a BA.
 *
 *  @param[in] host_id  Host ID.
 *
 *  @return Virtual BA host.
 */
std::shared_ptr<neb::host> applier::ba::_ba_host(uint32_t host_id) {
  std::shared_ptr<neb::host> h(new neb::host);
  h->poller_id =
      com::centreon::broker::config::applier::state::instance().poller_id();
  h->host_id = host_id;
  h->host_name = fmt::format("_Module_BAM_{}", h->poller_id);
  h->last_update = time(nullptr);
  return h;
}

/**
 *  Get the virtual BA service of a BA.
 *
 *  @param[in] ba_id       BA ID.
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 *
 *  @return Virtual BA service.
 */
std::shared_ptr<neb::service> applier::ba::_ba_service(uint32_t ba_id,
                                                       uint32_t host_id,
                                                       uint32_t service_id) {
  std::shared_ptr<neb::service> s(new neb::service);
  s->host_id = host_id;
  s->service_id = service_id;
  s->service_description = fmt::format("ba_{}", ba_id);
  s->last_update = time(nullptr);
  return s;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void applier::ba::_internal_copy(applier::ba const& other) {
  _applied = other._applied;
}

/**
 *  Create new BA object.
 *
 *  @param[in] cfg BA configuration.
 *
 *  @return New BA object.
 */
std::shared_ptr<bam::ba> applier::ba::_new_ba(configuration::ba const& cfg,
                                              service_book& book) {
  std::shared_ptr<bam::ba> obj(new bam::ba(false));
  obj->set_id(cfg.get_id());
  obj->set_host_id(cfg.get_host_id());
  obj->set_service_id(cfg.get_service_id());
  obj->set_name(cfg.get_name());
  obj->set_state_source(cfg.get_state_source());
  obj->set_level_warning(cfg.get_warning_level());
  obj->set_level_critical(cfg.get_critical_level());
  obj->set_downtime_behaviour(cfg.get_downtime_behaviour());
  if (cfg.get_opened_event().ba_id)
    obj->set_initial_event(cfg.get_opened_event());
  book.listen(cfg.get_host_id(), cfg.get_service_id(), obj.get());
  return obj;
}

/**
 *  Save inherited downtime to the cache.
 *
 *  @param[in] cache  The cache.
 */
void applier::ba::save_to_cache(persistent_cache& cache) {
  cache.transaction();
  for (std::map<uint32_t, applied>::const_iterator it = _applied.begin(),
                                                   end = _applied.end();
       it != end; ++it) {
    it->second.obj->save_inherited_downtime(cache);
  }
  cache.commit();
}

/**
 *  Load inherited downtime from cache.
 *
 *  @param[in] cache  The cache.
 */
void applier::ba::load_from_cache(persistent_cache& cache) {
  std::shared_ptr<io::data> d;
  cache.get(d);
  while (d) {
    if (d->type() != inherited_downtime::static_type())
      continue;
    inherited_downtime const& dwn =
        *std::static_pointer_cast<inherited_downtime const>(d);
    std::map<uint32_t, applied>::iterator found = _applied.find(dwn.ba_id);
    if (found != _applied.end()) {
      logging::debug(logging::medium)
          << "BAM: found an inherited downtime for BA " << found->first;
      found->second.obj->set_inherited_downtime(dwn);
    }
    cache.get(d);
  }
}
