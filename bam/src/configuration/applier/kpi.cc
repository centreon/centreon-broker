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

#include "com/centreon/broker/bam/configuration/applier/kpi.hh"
#include "com/centreon/broker/bam/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/applier/meta_service.hh"
#include "com/centreon/broker/bam/kpi_ba.hh"
#include "com/centreon/broker/bam/kpi_boolexp.hh"
#include "com/centreon/broker/bam/kpi_meta.hh"
#include "com/centreon/broker/bam/kpi_service.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/bam/meta_service.hh"
#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/exceptions/config.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::kpi::kpi()
    : _bas(nullptr), _book(nullptr), _boolexps(nullptr), _mapping(nullptr), _metas(nullptr) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
applier::kpi::kpi(applier::kpi const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
applier::kpi::~kpi() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
applier::kpi& applier::kpi::operator=(applier::kpi const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Apply configuration.
 *
 *  @param[in]     my_kpis      Object to copy.
 *  @param[in,out] my_bas       Already applied BAs.
 *  @param[in,out] my_metas     Already applied meta-services.
 *  @param[in,out] my_boolexps  Already applied boolean expressions.
 *  @param[out]    book         Service book.
 *
 *  @return This object.
 */
void applier::kpi::apply(bam::configuration::state::kpis const& my_kpis,
                         hst_svc_mapping const& mapping,
                         applier::ba& my_bas,
                         applier::meta_service& my_metas,
                         applier::bool_expression& my_boolexps,
                         bam::service_book& book) {
  // Set internal parameters.
  _mapping = &mapping;
  _bas = &my_bas;
  _metas = &my_metas;
  _boolexps = &my_boolexps;
  _book = &book;

  //
  // DIFF
  //

  // Objects to delete are items remaining in the
  // set at the end of the iteration.
  std::map<uint32_t, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  bam::configuration::state::kpis to_create(my_kpis);

  // Iterate through configuration.
  for (bam::configuration::state::kpis::iterator it(to_create.begin()),
       end(to_create.end());
       it != end;) {
    std::map<uint32_t, applied>::iterator cfg_it(to_delete.find(it->first));
    // Found = modify (or not).
    if (cfg_it != to_delete.end()) {
      // Configuration mismatch, modify object
      // (indeed deletion + recreation).
      if (cfg_it->second.cfg != it->second)
        ++it;
      else {
        to_delete.erase(cfg_it);
        bam::configuration::state::kpis::iterator tmp = it;
        ++it;
        to_create.erase(tmp);
      }
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
        << "BAM: removing KPI " << it->second.cfg.get_id();
    _remove_kpi(it->first);
  }
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::kpis::iterator it(to_create.begin()),
       end(to_create.end());
       it != end; ++it) {
    if (!mapping.get_activated(it->second.get_host_id(),
                               it->second.get_service_id())) {
      logging::info(logging::medium) << "BAM: ignoring kpi '" << it->first
                                     << "' linked to a deactivated service";
      continue;
    }
    try {
      std::shared_ptr<bam::kpi> new_kpi(_new_kpi(it->second));
      applied& content(_applied[it->first]);
      content.cfg = it->second;
      content.obj = new_kpi;
    } catch (exceptions::config const& e) {
      // Log message.
      logging::error(logging::high)
          << "BAM: could not create KPI " << it->first << ": " << e.what();

      _invalidate_ba(it->second);
    }
  }

  //
  // OBJECT RESOLUTION
  //
  for (std::map<uint32_t, applied>::const_iterator kpi_it(_applied.begin()),
       next_kpi_it(_applied.begin()), kpi_end(_applied.end());
       kpi_it != kpi_end; kpi_it = next_kpi_it) {
    ++next_kpi_it;
    configuration::kpi const& cfg(kpi_it->second.cfg);
    std::shared_ptr<bam::kpi> my_kpi(kpi_it->second.obj);
    try {
      _resolve_kpi(cfg, my_kpi);
    } catch (exceptions::config const& e) {
      // Log message.
      logging::error(logging::high)
          << "BAM: could not resolve KPI " << cfg.get_id() << ": " << e.what();

      _invalidate_ba(cfg);
    }
  }

  return;
}

/**
 *  Invalidates the ba associated to the kpi and also removes this kpi.
 *
 *  @param kpi The Kpi configuration.
 */
void applier::kpi::_invalidate_ba(configuration::kpi const& kpi) {
  // Set KPI as invalid.
  {
    std::shared_ptr<kpi_status> ks(new kpi_status);
    ks->kpi_id = kpi.get_id();
    ks->state_hard = 3;
    ks->state_soft = 3;
    ks->level_acknowledgement_hard = 0.0;
    ks->level_acknowledgement_soft = 0.0;
    ks->level_downtime_hard = 0.0;
    ks->level_downtime_soft = 0.0;
    ks->level_nominal_hard = 0.0;
    ks->level_nominal_soft = 0.0;
    ks->last_state_change = time(nullptr);
    ks->valid = false;
    multiplexing::publisher().write(ks);
  }

  // Right now we have an invalid KPI targeting a BA. To avoid
  // computation errors we now have to remove all KPIs that are
  // targeting this same BA and that are already applied. Eventual
  // remaining KPIs that are targeting the BA will be discarded
  // because the BA won't be found. The BA object will remain
  // applied.

  // Remove KPIs linked to BA of this KPI.
  for (std::map<uint32_t, applied>::const_iterator kpi_it(_applied.begin()),
       kpi_end(_applied.end());
       kpi_it != kpi_end;) {
    if (kpi_it->second.cfg.get_ba_id() == kpi.get_ba_id()) {
      uint32_t kpi_id(kpi_it->first);
      ++kpi_it;
      _remove_kpi(kpi_id);
    } else
      ++kpi_it;
  }

  // Set BA as invalid.
  std::shared_ptr<bam::ba> my_ba(_bas->find_ba(kpi.get_ba_id()));
  if (my_ba)
    my_ba->set_valid(false);

  return;
}

/**
 *  Visit KPIs.
 *
 *  @param[out] visitor  Object that will receive status.
 */
void applier::kpi::visit(io::stream* visitor) {
  for (std::map<uint32_t, applied>::iterator it(_applied.begin()),
       end(_applied.end());
       it != end; ++it)
    it->second.obj->visit(visitor);
  return;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void applier::kpi::_internal_copy(applier::kpi const& other) {
  _applied = other._applied;
  _bas = other._bas;
  _book = other._book;
  _boolexps = other._boolexps;
  _mapping = other._mapping;
  _metas = other._metas;
  return;
}

/**
 *  Create new KPI object.
 *
 *  @param[in] cfg  KPI configuration.
 *
 *  @return New KPI object.
 */
std::shared_ptr<bam::kpi> applier::kpi::_new_kpi(
    configuration::kpi const& cfg) {
  // Create KPI according to its type.
  std::shared_ptr<bam::kpi> my_kpi;
  if (cfg.is_service()) {
    logging::config(logging::medium)
        << "BAM: creating new KPI " << cfg.get_id() << " of service ("
        << cfg.get_host_id() << ", " << cfg.get_service_id()
        << ") impacting BA " << cfg.get_ba_id();
    std::shared_ptr<bam::kpi_service> obj(new bam::kpi_service);
    obj->set_acknowledged(cfg.is_acknowledged());
    obj->set_downtimed(cfg.is_downtimed());
    obj->set_host_id(cfg.get_host_id());
    obj->set_impact_critical(cfg.get_impact_critical());
    obj->set_impact_unknown(cfg.get_impact_unknown());
    obj->set_impact_warning(cfg.get_impact_warning());
    obj->set_service_id(cfg.get_service_id());
    obj->set_state_hard(static_cast<bam::kpi_service::state>(cfg.get_status()));
    obj->set_state_type(cfg.get_state_type());
    _book->listen(cfg.get_host_id(), cfg.get_service_id(), obj.get());
    my_kpi = std::static_pointer_cast<bam::kpi>(obj);
  } else if (cfg.is_ba()) {
    logging::config(logging::medium)
        << "BAM: creating new KPI " << cfg.get_id() << " of BA "
        << cfg.get_indicator_ba_id() << " impacting BA " << cfg.get_ba_id();
    std::shared_ptr<bam::kpi_ba> obj(new bam::kpi_ba);
    obj->set_impact_critical(cfg.get_impact_critical());
    obj->set_impact_warning(cfg.get_impact_warning());
    my_kpi = std::static_pointer_cast<bam::kpi>(obj);
  } else if (cfg.is_meta()) {
    std::shared_ptr<bam::kpi_meta> obj(new bam::kpi_meta);
    logging::config(logging::medium)
        << "BAM: creating new KPI " << cfg.get_id() << " of meta-service "
        << cfg.get_meta_id() << " impacting BA " << cfg.get_ba_id();
    obj->set_impact_critical(cfg.get_impact_critical());
    obj->set_impact_warning(cfg.get_impact_warning());
    my_kpi = std::static_pointer_cast<bam::kpi>(obj);
  } else if (cfg.is_boolexp()) {
    logging::config(logging::medium)
        << "BAM: creating new KPI " << cfg.get_id() << " of boolean expression "
        << cfg.get_boolexp_id() << " impacting BA " << cfg.get_ba_id();
    std::shared_ptr<bam::kpi_boolexp> obj(new bam::kpi_boolexp);
    obj->set_impact(cfg.get_impact_critical());
    my_kpi = std::static_pointer_cast<bam::kpi>(obj);
  } else
    throw(exceptions::config()
          << "create KPI " << cfg.get_id()
          << " is neither related to a service, nor a BA,"
          << " nor a meta-service, nor a boolean expression");

  my_kpi->set_id(cfg.get_id());

  return (my_kpi);
}

/**
 *  Associates the KPI to its targets (also BA)
 *
 * @param cfg The kpi configuration
 * @param kpi The Kpi itself.
 */
void applier::kpi::_resolve_kpi(configuration::kpi const& cfg,
                                std::shared_ptr<bam::kpi> kpi) {
  // Find target BA.
  std::shared_ptr<bam::ba> my_ba(_bas->find_ba(cfg.get_ba_id()));
  if (!my_ba)
    throw(exceptions::config()
          << "target BA " << cfg.get_ba_id() << " does not exist");

  if (cfg.is_ba()) {
    std::shared_ptr<bam::kpi_ba> obj(
        std::static_pointer_cast<bam::kpi_ba>(kpi));
    std::shared_ptr<bam::ba> target(_bas->find_ba(cfg.get_indicator_ba_id()));
    if (!target)
      throw(exceptions::config()
            << "could not find source BA " << cfg.get_indicator_ba_id());
    obj->link_ba(target);
    target->add_parent(std::static_pointer_cast<bam::computable>(obj));
    logging::config(logging::medium)
        << "BAM: Resolve KPI " << kpi->get_id() << " connections to its BA";
  } else if (cfg.is_meta()) {
    std::shared_ptr<bam::kpi_meta> obj(
        std::static_pointer_cast<bam::kpi_meta>(kpi));
    std::shared_ptr<bam::meta_service> target(
        _metas->find_meta(cfg.get_meta_id()));
    if (!target)
      throw(exceptions::config()
            << "could not find source meta-service " << cfg.get_meta_id());
    obj->link_meta(target);
    target->add_parent(std::static_pointer_cast<bam::computable>(obj));
    logging::config(logging::medium) << "BAM: Resolve KPI " << kpi->get_id()
                                     << " connections to its meta-service";
  } else if (cfg.is_boolexp()) {
    std::shared_ptr<bam::kpi_boolexp> obj(
        std::static_pointer_cast<bam::kpi_boolexp>(kpi));
    std::shared_ptr<bam::bool_expression> target(
        _boolexps->find_boolexp(cfg.get_boolexp_id()));
    if (!target)
      throw(exceptions::config() << "could not find source boolean expression "
                                 << cfg.get_boolexp_id());
    obj->link_boolexp(target);
    target->add_parent(std::static_pointer_cast<bam::computable>(obj));
    logging::config(logging::medium)
        << "BAM: Resolve KPI " << kpi->get_id()
        << " connections to its boolean expression";
  }

  // Link KPI with BA.
  if (cfg.get_opened_event().kpi_id != 0)
    kpi->set_initial_event(cfg.get_opened_event());

  my_ba->add_impact(std::static_pointer_cast<bam::kpi>(kpi));
  kpi->add_parent(std::static_pointer_cast<bam::computable>(my_ba));
}

/**
 *  Remove an applied KPI.
 *
 *  @param[in] kpi_id  KPI ID.
 */
void applier::kpi::_remove_kpi(uint32_t kpi_id) {
  std::map<uint32_t, applied>::iterator it(_applied.find(kpi_id));
  if (it != _applied.end()) {
    if (it->second.cfg.is_service())
      _book->unlisten(it->second.cfg.get_host_id(),
                      it->second.cfg.get_service_id(),
                      static_cast<kpi_service*>(it->second.obj.get()));
    std::shared_ptr<bam::ba> my_ba(_bas->find_ba(it->second.cfg.get_ba_id()));
    if (my_ba)
      my_ba->remove_impact(it->second.obj);
    _applied.erase(it);
  }
  return;
}
