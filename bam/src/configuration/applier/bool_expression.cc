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

#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/bool_expression.hh"
#include "com/centreon/broker/bam/bool_parser.hh"
#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/bool_expression.hh"
#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::bool_expression::bool_expression() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
applier::bool_expression::bool_expression(
                            applier::bool_expression const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
applier::bool_expression::~bool_expression() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
applier::bool_expression& applier::bool_expression::operator=(
                                                      applier::bool_expression const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Apply boolean expressions.
 *
 *  @param[in]     my_bools Boolean expressions.
 *  @param[in]     mapping  Host/service mapping (names to IDs).
 *  @param[in,out] my_bas   BAs on which boolean expressions will be
 *                          applied.
 *  @param[out]    book     Used to notify bool_service of service
 *                          change.
 */
void applier::bool_expression::apply(
                                 configuration::state::bool_exps const& my_bools,
                                 hst_svc_mapping const& mapping,
                                 ba& my_bas,
                                 service_book& book) {
  //
  // DIFF
  //

  // Objects to delete are items remaining in the
  // set at the end of the iteration.
  std::map<unsigned int, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  bam::configuration::state::bool_exps to_create(my_bools);

  // Objects to modify are items found but
  // with mismatching configuration.
  std::list<bam::configuration::bool_expression> to_modify;

  // Iterate through configuration.
  for (bam::configuration::state::bool_exps::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;) {
    std::map<unsigned int, applied>::iterator
      cfg_it(to_delete.find(it->get_id()));
    // Found = modify (or not).
    if (cfg_it != to_delete.end()) {
      // Configuration mismatch, modify object.
      if (cfg_it->second.cfg != *it)
        to_modify.push_back(*it);
      to_delete.erase(cfg_it);
      it = to_create.erase(it);
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
    for (bam::configuration::bool_expression::ids_of_bas::const_iterator
           it2(it->second.cfg.get_impacted_bas().begin()),
           end2(it->second.cfg.get_impacted_bas().end());
         it2 != end2;
         ++it2) {
      misc::shared_ptr<bam::ba> target(my_bas.find_ba(*it2));
      if (!target.isNull())
        target->remove_impact(it->second.obj.staticCast<bam::kpi>());
    }
    for (std::list<bool_service::ptr>::const_iterator
           it2(it->second.svc.begin()),
           end2(it->second.svc.end());
         it2 != end2;
         ++it2)
      book.unlisten(
             (*it2)->get_host_id(),
             (*it2)->get_service_id(),
             it2->data());
    _applied.erase(it->first);
  }
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::bool_exps::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    misc::shared_ptr<bam::bool_expression>
      new_bool_exp(new bam::bool_expression);
    {
      bam::bool_parser p(it->get_expression(), mapping);
      new_bool_exp->set_expression(p.get_tree());
      applied& content(_applied[it->get_id()]);
      content.cfg = *it;
      content.obj = new_bool_exp;
      content.svc = p.get_services();
      for (std::list<bool_service::ptr>::const_iterator
             it2(content.svc.begin()),
             end2(content.svc.end());
           it2 != end2;
           ++it2)
        book.listen(
               (*it2)->get_host_id(),
               (*it2)->get_service_id(),
               it2->data());
    }
    new_bool_exp->set_id(it->get_id());
    new_bool_exp->set_impact_hard(it->get_impact());
    new_bool_exp->set_impact_if(it->get_impact_if());
    new_bool_exp->set_impact_soft(it->get_impact());
    for (bam::configuration::bool_expression::ids_of_bas::const_iterator
           it2(it->get_impacted_bas().begin()),
           end2(it->get_impacted_bas().end());
         it2 != end2;
         ++it2) {
      misc::shared_ptr<bam::ba> target(my_bas.find_ba(*it2));
      if (target.isNull())
        logging::config(logging::high) << "BAM: could not find BA "
          << *it2 << " for boolean expression " << it->get_id()
          << ", BA won't be impacted by this boolean expression";
      target->add_impact(new_bool_exp.staticCast<bam::kpi>());
    }
  }

  // Modify existing objects.
  // XXX
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void applier::bool_expression::_internal_copy(
                                 applier::bool_expression const& other) {
  _applied = other._applied;
  return ;
}
