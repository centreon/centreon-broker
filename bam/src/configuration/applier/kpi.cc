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
** <http:/www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/kpi.hh"
#include "com/centreon/broker/bam/kpi_ba.hh"
#include "com/centreon/broker/bam/kpi_service.hh"
#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::kpi::kpi() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
applier::kpi::kpi(applier::kpi const& right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
applier::kpi::~kpi() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
applier::kpi& applier::kpi::operator=(applier::kpi const& right) {
  if (this != &right)
    _internal_copy(right);
  return (*this);
}

/**
 *  Apply configuration.
 *
 *  @param[in]     my_kpis Object to copy.
 *  @param[in,out] my_bas  Already applied BAs.
 *  @param[out]    book    Service book.
 *
 *  @return This object.
 */
void applier::kpi::apply(
                     bam::configuration::state::kpis const& my_kpis,
                     applier::ba& my_bas,
                     bam::service_book& book) {
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
      cfg_it(to_delete.find(it->get_id()));
    // Found = modify (or not).
    if (cfg_it != to_delete.end()) {
      // Configuration mismatch, modify object
      // (indeed deletion + recreation).
      if (cfg_it->second.cfg != *it)
        ++it;
      else {
        to_delete.erase(cfg_it);
        it = to_create.erase(it);
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
    if (it->second.cfg.is_service())
      book.unlisten(
             it->second.cfg.get_host_id(),
             it->second.cfg.get_service_id(),
             static_cast<kpi_service*>(it->second.obj.data()));
    _applied.erase(it->first);
  }
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::kpis::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    misc::shared_ptr<bam::kpi> new_kpi(_new_kpi(*it, my_bas, book));
    applied& content(_applied[it->get_id()]);
    content.cfg = *it;
    content.obj = new_kpi;
  }

  return ;
}

/**
 *  Visit KPIs.
 *
 *  @param[out] visitor  Object that will receive status.
 */
void applier::kpi::visit(stream* visitor) {
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
 *  @param[in] right Object to copy.
 */
void applier::kpi::_internal_copy(applier::kpi const& right) {
  _applied = right._applied;
  return ;
}

/**
 *  Create new KPI object.
 *
 *  @param[in]     cfg    KPI configuration.
 *  @param[in,out] my_bas Already applied BAs.
 *  @param[out]    book   Service book, used to notify kpi_service of
 *                        service change.
 *
 *  @return New KPI object.
 */
misc::shared_ptr<bam::kpi> applier::kpi::_new_kpi(
                                           configuration::kpi const& cfg,
                                           applier::ba& my_bas,
                                           service_book& book) {
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
    obj->set_state_hard(cfg.get_last_hard_state());
    obj->set_state_soft(cfg.get_status());
    obj->set_state_type(cfg.get_state_type());
    my_kpi = obj.staticCast<bam::kpi>();
    book.listen(cfg.get_host_id(), cfg.get_service_id(), obj.data());
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
      target(my_bas.find_ba(cfg.get_indicator_ba_id()));
    if (target.isNull())
      throw (exceptions::msg()
             << "BAM: could not create KPI " << cfg.get_id()
             << "): could not find BA " << cfg.get_indicator_ba_id());
    obj->link_ba(target);
    target->add_parent(obj.staticCast<bam::computable>());
    my_kpi = obj.staticCast<bam::kpi>();
  }
  else
    throw (exceptions::msg()
           << "BAM: could not create KPI " << cfg.get_id()
           << " which is neither related to a service nor a BA");
  misc::shared_ptr<bam::ba>
    my_ba(my_bas.find_ba(cfg.get_ba_id()));
  if (my_ba.isNull())
    throw (exceptions::msg()
           << "BAM: could not create KPI " << cfg.get_id()
           << ": BA " << cfg.get_ba_id() << " does not exist");
  my_kpi->set_id(cfg.get_id());
  my_ba->add_impact(my_kpi.staticCast<bam::kpi>());
  my_kpi->add_ba(my_ba);
  return (my_kpi);
}
