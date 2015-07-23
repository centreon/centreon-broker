/*
** Copyright 2014-2015 Merethis
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
** <http:/www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/applier/kpi.hh"
#include "com/centreon/broker/bam/configuration/applier/meta_service.hh"
#include "com/centreon/broker/bam/bool_expression.hh"
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
  : _bas(NULL),
    _book(NULL),
    _boolexps(NULL),
    _mapping(NULL),
    _metas(NULL) {}

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
void applier::kpi::apply(
                     bam::configuration::state::kpis const& my_kpis,
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
  std::map<unsigned int, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  bam::configuration::state::kpis to_create(my_kpis);

  // Iterate through configuration.
  for (bam::configuration::state::kpis::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;) {
    std::map<unsigned int, applied>::iterator
      cfg_it(to_delete.find(it->first));
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
  for (std::map<unsigned int, applied>::iterator
         it(to_delete.begin()),
         end(to_delete.end());
       it != end;
       ++it) {
    logging::config(logging::medium)
      << "BAM: removing KPI " << it->second.cfg.get_id();
    _remove_kpi(it->first);
  }
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::kpis::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    if (!mapping.get_activated(
                   it->second.get_host_id(),
                   it->second.get_service_id())) {
      logging::info(logging::medium)
        << "BAM: ignoring kpi '" << it->first
        << "' linked to a deactivated service";
      continue;
    }
    try {
      misc::shared_ptr<bam::kpi> new_kpi(_new_kpi(it->second));
      applied& content(_applied[it->first]);
      content.cfg = it->second;
      content.obj = new_kpi;
    }
    catch (exceptions::config const& e) {
      // Log message.
      logging::error(logging::high)
        << "BAM: could not create KPI " << it->first << ": "
        << e.what();

      // Set KPI as invalid.
      {
        misc::shared_ptr<kpi_status> ks(new kpi_status);
        ks->kpi_id = it->first;
        ks->state_hard = 3;
        ks->state_soft = 3;
        ks->level_acknowledgement_hard = 0.0;
        ks->level_acknowledgement_soft = 0.0;
        ks->level_downtime_hard = 0.0;
        ks->level_downtime_soft = 0.0;
        ks->level_nominal_hard = 0.0;
        ks->level_nominal_soft = 0.0;
        ks->last_state_change = time(NULL);
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
      for (std::map<unsigned int, applied>::const_iterator
             kpi_it(_applied.begin()),
             kpi_end(_applied.end());
           kpi_it != kpi_end;) {
        if (kpi_it->second.cfg.get_ba_id() == it->second.get_ba_id()) {
          unsigned int kpi_id(kpi_it->first);
          ++kpi_it;
          _remove_kpi(kpi_id);
        }
        else
          ++kpi_it;
      }

      // Set BA as invalid.
      misc::shared_ptr<bam::ba>
        my_ba(_bas->find_ba(it->second.get_ba_id()));
      if (!my_ba.isNull())
        my_ba->set_valid(false);
    }
  }

  return ;
}

/**
 *  Visit KPIs.
 *
 *  @param[out] visitor  Object that will receive status.
 */
void applier::kpi::visit(io::stream* visitor) {
  for (std::map<unsigned int, applied>::iterator
         it(_applied.begin()),
         end(_applied.end());
       it != end;
       ++it)
    it->second.obj->visit(visitor);
  return ;
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
  return ;
}

/**
 *  Create new KPI object.
 *
 *  @param[in] cfg  KPI configuration.
 *
 *  @return New KPI object.
 */
misc::shared_ptr<bam::kpi> applier::kpi::_new_kpi(
                                           configuration::kpi const& cfg) {
  // Find target BA.
  misc::shared_ptr<bam::ba>
    my_ba(_bas->find_ba(cfg.get_ba_id()));
  if (my_ba.isNull())
    throw (exceptions::config()
           << "target BA " << cfg.get_ba_id() << " does not exist");

  // Create KPI according to its type.
  misc::shared_ptr<bam::kpi> my_kpi;
  if (cfg.is_service()) {
    logging::config(logging::medium)
      << "BAM: creating new KPI " << cfg.get_id() << " of service ("
      << cfg.get_host_id() << ", " << cfg.get_service_id()
      << ") impacting BA " << cfg.get_ba_id();
    misc::shared_ptr<bam::kpi_service> obj(new bam::kpi_service);
    obj->set_acknowledged(cfg.is_acknowledged());
    obj->set_downtimed(cfg.is_downtimed());
    obj->set_host_id(cfg.get_host_id());
    obj->set_impact_critical(cfg.get_impact_critical());
    obj->set_impact_unknown(cfg.get_impact_unknown());
    obj->set_impact_warning(cfg.get_impact_warning());
    obj->set_service_id(cfg.get_service_id());
    obj->set_state_hard(cfg.get_status());
    obj->set_state_type(cfg.get_state_type());
    my_kpi = obj.staticCast<bam::kpi>();
    _book->listen(cfg.get_host_id(), cfg.get_service_id(), obj.data());
  }
  else if (cfg.is_ba()) {
    logging::config(logging::medium)
      << "BAM: creating new KPI " << cfg.get_id() << " of BA "
      << cfg.get_indicator_ba_id() << " impacting BA "
      << cfg.get_ba_id();
    misc::shared_ptr<bam::kpi_ba> obj(new bam::kpi_ba);
    obj->set_impact_critical(cfg.get_impact_critical());
    obj->set_impact_warning(cfg.get_impact_warning());
    misc::shared_ptr<bam::ba>
      target(_bas->find_ba(cfg.get_indicator_ba_id()));
    if (target.isNull())
      throw (exceptions::config() << "could not find source BA "
             << cfg.get_indicator_ba_id());
    obj->link_ba(target);
    target->add_parent(obj.staticCast<bam::computable>());
    my_kpi = obj.staticCast<bam::kpi>();
  }
  else if (cfg.is_meta()) {
    logging::config(logging::medium)
      << "BAM: creating new KPI " << cfg.get_id() << " of meta-service "
      << cfg.get_meta_id() << " impacting BA " << cfg.get_ba_id();
    misc::shared_ptr<bam::kpi_meta> obj(new bam::kpi_meta);
    obj->set_impact_critical(cfg.get_impact_critical());
    obj->set_impact_warning(cfg.get_impact_warning());
    misc::shared_ptr<bam::meta_service>
      target(_metas->find_meta(cfg.get_meta_id()));
    if (target.isNull())
      throw (exceptions::config()
             << "could not find source meta-service "
             << cfg.get_meta_id());
    obj->link_meta(target);
    target->add_parent(obj.staticCast<bam::computable>());
    my_kpi = obj.staticCast<bam::kpi>();
  }
  else if (cfg.is_boolexp()) {
    logging::config(logging::medium)
      << "BAM: creating new KPI " << cfg.get_id()
      << " of boolean expression " << cfg.get_boolexp_id()
      << " impacting BA " << cfg.get_ba_id();
    misc::shared_ptr<bam::kpi_boolexp> obj(new bam::kpi_boolexp);
    obj->set_impact(cfg.get_impact_critical());
    misc::shared_ptr<bam::bool_expression>
      target(_boolexps->find_boolexp(cfg.get_boolexp_id()));
    if (target.isNull())
      throw (exceptions::config()
             << "could not find source boolean expression "
             << cfg.get_boolexp_id());
    obj->link_boolexp(target);
    target->add_parent(obj.staticCast<bam::computable>());
    my_kpi = obj.staticCast<bam::kpi>();
  }
  else
    throw (exceptions::config() << "create KPI " << cfg.get_id()
           << " is neither related to a service, nor a BA,"
           << " nor a meta-service, nor a boolean expression");

  // Set common KPI parameters.
  my_kpi->set_id(cfg.get_id());
  if (cfg.get_opened_event().kpi_id != 0)
    my_kpi->set_initial_event(cfg.get_opened_event());

  // Link KPI with BA.
  my_ba->add_impact(my_kpi.staticCast<bam::kpi>());
  my_kpi->add_parent(my_ba.staticCast<bam::computable>());

  return (my_kpi);
}

/**
 *  Remove an applied KPI.
 *
 *  @param[in] kpi_id  KPI ID.
 */
void applier::kpi::_remove_kpi(unsigned int kpi_id) {
  std::map<unsigned int, applied>::iterator
    it(_applied.find(kpi_id));
  if (it != _applied.end()) {
    if (it->second.cfg.is_service())
      _book->unlisten(
               it->second.cfg.get_host_id(),
               it->second.cfg.get_service_id(),
               static_cast<kpi_service*>(it->second.obj.data()));
    misc::shared_ptr<bam::ba>
      my_ba(_bas->find_ba(it->second.cfg.get_ba_id()));
    if (!my_ba.isNull())
      my_ba->remove_impact(it->second.obj);
    _applied.erase(it);
  }
  return ;
}
