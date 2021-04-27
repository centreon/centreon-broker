/*
** Copyright 2014-2015, 2021 Centreon
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
#include <fmt/format.h>
#include "com/centreon/broker/bam/metric_book.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/service.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::meta_service::meta_service() {}

void applier::meta_service::apply(const state::meta_services& my_meta,
                                  metric_book& book) {
  /* We make the diff between the running configuration and the new one */
  std::list<uint32_t> to_delete;
  std::list<const configuration::meta_service*> to_create;
  std::list<configuration::meta_service*> to_modify;

  for (auto it = _applied.begin(), end = _applied.end(); it != end; ++it) {
    auto found = my_meta.find(it->first);
    if (found == my_meta.end())
      // We should delete this meta.
      to_delete.push_back(it->first);
    else if (it->second.cfg != found->second) {
      // We should keep it but configurations mismatch, modify object.
      to_modify.push_back(&it->second.cfg);
    }
  }

  for (auto it = my_meta.begin(), end = my_meta.end(); it != end; ++it) {
    if (_applied.find(it->first) == _applied.end())
      to_create.push_back(&it->second);
  }

  /* Time to delete objects. */
  for (uint32_t meta_id : to_delete) {
    log_v2::bam()->info("BAM: removing meta-service {}", meta_id);
    auto it = _applied.find(meta_id);
    std::shared_ptr<neb::service> s{
        _meta_service(meta_id, it->second.cfg.get_host_id(),
                      it->second.cfg.get_service_id())};
    s->enabled = false;
    book.unlisten(it->second.cfg.get_id(), it->second.obj.get());
    _applied.erase(meta_id);
    multiplexing::publisher().write(s);
  }

  /* Time to create new objects. */
  for (auto* tc : to_create) {
    log_v2::bam()->info("BAM: creating meta-service {}", tc->get_id());
    _applied.insert({tc->get_id(), {.cfg = *tc, .obj = _new_meta(*tc, book)}});
    std::shared_ptr<neb::host> h(_meta_host(tc->get_host_id()));
    multiplexing::publisher().write(h);
    std::shared_ptr<neb::service> s(
        _meta_service(tc->get_id(), tc->get_host_id(), tc->get_service_id()));
    multiplexing::publisher().write(s);
  }

  /* Time to modify existing objects. */
  for (auto* m : to_modify) {
    std::map<uint32_t, applied>::iterator pos(_applied.find(m->get_id()));
    if (pos != _applied.end()) {
      log_v2::bam()->info("BAM: modifying meta-service {}", m->get_id());
      _modify_meta(*pos->second.obj, book, &pos->second.cfg, m);
      pos->second.cfg = *m;
    } else
      log_v2::bam()->error(
          "BAM: attempting to modify meta-service {}, however associated "
          "object was not found. This is likely a software bug that you should "
          "report to Centreon Broker developers",
          m->get_id());
  }
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
  return (it != _applied.end()) ? it->second.obj
                                : std::shared_ptr<bam::meta_service>();
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
  return h;
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
  s->service_description = fmt::format("meta_{}", meta_id);
  s->last_update = time(nullptr);
  return s;
}

/**
 *  Modify a meta-service.
 *
 *  @param[in,out] obj      Meta-service object.
 *  @param[in,out] book     Metric book.
 *  @param[in]     old_cfg  Old configuration. If nullptr, no old config.
 *  @param[in]     new_cfg  New configuration.
 */
void applier::meta_service::_modify_meta(
    bam::meta_service& obj,
    metric_book& book,
    configuration::meta_service const* old_cfg,
    configuration::meta_service const* new_cfg) {
  // Remove old metrics from 1) the book and from 2) the meta-service.
  if (old_cfg) {
    for (configuration::meta_service::metric_container::const_iterator
             it(old_cfg->get_metrics().begin()),
         end(old_cfg->get_metrics().end());
         it != end; ++it) {
      log_v2::bam()->info(
          "BAM: meta-service {} does not depend on metric {} anymore",
          obj.get_id(), *it);
      book.unlisten(*it, &obj);
      obj.remove_metric(*it);
    }
  }

  // Add new metrics to 1) the book and to 2) the meta-service.
  for (configuration::meta_service::metric_container::const_iterator
           it(new_cfg->get_metrics().begin()),
       end(new_cfg->get_metrics().end());
       it != end; ++it) {
    log_v2::bam()->info("BAM: meta-service {} uses metric {}", obj.get_id(),
                        *it);
    book.listen(*it, &obj);
    obj.add_metric(*it);
  }

  // Modify meta-service properties.
  std::string const& computation_str(new_cfg->get_computation());
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
  obj.set_level_warning(new_cfg->get_level_warning());
  obj.set_level_critical(new_cfg->get_level_critical());
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
  std::shared_ptr<bam::meta_service> meta{std::make_shared<bam::meta_service>(
      cfg.get_host_id(), cfg.get_service_id(), cfg.get_id())};
  _modify_meta(*meta, book, nullptr, &cfg);
  return meta;
}
