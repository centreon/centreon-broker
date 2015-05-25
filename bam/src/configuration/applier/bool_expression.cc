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
 *  @param[out]    book     Used to notify bool_service of service
 *                          change.
 */
void applier::bool_expression::apply(
                                 configuration::state::bool_exps const& my_bools,
                                 hst_svc_mapping const& mapping,
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

  // Iterate through configuration.
  for (bam::configuration::state::bool_exps::iterator
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
        bam::configuration::state::bool_exps::iterator tmp = it;
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
      << "BAM: removing boolean expression " << it->second.cfg.get_id();
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
    logging::config(logging::medium)
      << "BAM: creating new boolean expression " << it->first;
    misc::shared_ptr<bam::bool_expression>
      new_bool_exp(new bam::bool_expression);
    {
      bam::bool_parser p(it->second.get_expression(), mapping);
      bam::bool_value::ptr tree(p.get_tree());
      new_bool_exp->set_expression(tree);
      if (!tree.isNull())
        tree->add_parent(new_bool_exp.staticCast<bam::computable>());
      applied& content(_applied[it->first]);
      content.cfg = it->second;
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
    new_bool_exp->set_id(it->first);
    new_bool_exp->set_impact_if(it->second.get_impact_if());
  }

  return ;
}

/**
 *  Find a boolean expression by its ID.
 *
 *  @param[in] id  Boolean expression ID.
 *
 *  @return Shared pointer to the applied boolean expression object.
 */
misc::shared_ptr<bam::bool_expression> applier::bool_expression::find_boolexp(
                                                                   unsigned int id) {
  std::map<unsigned int, applied>::iterator
    it(_applied.find(id));
  return ((it != _applied.end())
          ? it->second.obj
          : misc::shared_ptr<bam::bool_expression>());
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
