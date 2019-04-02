/*
** Copyright 2014-2016 Centreon
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

#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/bool_expression.hh"
#include "com/centreon/broker/bam/exp_builder.hh"
#include "com/centreon/broker/bam/exp_parser.hh"
#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/bam/metric_book.hh"
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
 *  @param[out]    metric_book  Used to notify bool_metric of metric change.
 */
void applier::bool_expression::apply(
                                 configuration::state::bool_exps const& my_bools,
                                 hst_svc_mapping const& mapping,
                                 service_book& book,
                                 metric_book& metric_book) {
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
             it2->get());
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
    std::shared_ptr<bam::bool_expression>
      new_bool_exp(new bam::bool_expression);
    try {
      bam::exp_parser p(it->second.get_expression());
      bam::exp_builder b(p.get_postfix(), mapping);
      bam::bool_value::ptr tree(b.get_tree());
      new_bool_exp->set_expression(tree);
      if (tree)
        tree->add_parent(std::static_pointer_cast<bam::computable>(new_bool_exp));
      applied& content(_applied[it->first]);
      content.cfg = it->second;
      content.obj = new_bool_exp;
      content.svc = b.get_services();
      content.call = b.get_calls();
      content.mtrc = b.get_metrics();
      // Resolve boolean service.
      for (std::list<bool_service::ptr>::const_iterator
             it2(content.svc.begin()),
             end2(content.svc.end());
           it2 != end2;
           ++it2)
        book.listen(
               (*it2)->get_host_id(),
               (*it2)->get_service_id(),
               it2->get());
      // Resolve boolean metric.
      for (std::list<bool_metric::ptr>::const_iterator
             it2 = content.mtrc.begin(),
             end2 = content.mtrc.end();
           it2 != end2;
           ++it2) {
        (*it2)->resolve_metrics(mapping);
        std::set<unsigned int> const& ids
          = (*it2)->get_resolved_metrics();
        for (std::set<unsigned int>::const_iterator
               metrics_it = ids.begin(),
               metrics_end = ids.end();
             metrics_it != metrics_end;
             ++metrics_it)
          metric_book.listen(
                      *metrics_it,
                      it2->get());
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "BAM: could not create boolean expression "
        << it->first << " so it will be discarded: " << e.what();
    }
    new_bool_exp->set_id(it->first);
    new_bool_exp->set_impact_if(it->second.get_impact_if());
  }

  _resolve_expression_calls();
  return ;
}

/**
 *  Find a boolean expression by its ID.
 *
 *  @param[in] id  Boolean expression ID.
 *
 *  @return Shared pointer to the applied boolean expression object.
 */
std::shared_ptr<bam::bool_expression> applier::bool_expression::find_boolexp(
                                                                   unsigned int id) {
  std::map<unsigned int, applied>::iterator
    it(_applied.find(id));
  return ((it != _applied.end())
          ? it->second.obj
          : std::shared_ptr<bam::bool_expression>());
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

/**
 *  Resolve the cross
 */
void applier::bool_expression::_resolve_expression_calls() {
  std::map<std::string, unsigned int>
    _name_to_ids;
  for (std::map<unsigned int, applied>::const_iterator
         it = _applied.begin(),
         end = _applied.end();
       it != end;
       ++it)
    _name_to_ids[it->second.cfg.get_name()] = it->first;

  for (std::map<unsigned int, applied>::iterator
         it = _applied.begin(),
         tmp = it,
         end = _applied.end();
       it != end;
       it = tmp) {
    ++tmp;
    for (std::list<std::shared_ptr<bam::bool_call> >::iterator
           call_it = it->second.call.begin(),
           call_end = it->second.call.end();
         call_it != call_end;
         ++call_it) {
      std::map<std::string, unsigned int>::const_iterator
        found = _name_to_ids.find((*call_it)->get_name());
      if (found == _name_to_ids.end()) {
        logging::error(logging::high)
          << "BAM: could not resolve the external boolean called '"
          << (*call_it)->get_name() << "' for expression '"
          << it->second.cfg.get_name() << "'";
        break;
      }
      else {
        (*call_it)->set_expression(_applied[found->second].obj->get_expression());
      }
    }
  }
}
