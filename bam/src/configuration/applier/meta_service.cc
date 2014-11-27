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

#include "com/centreon/broker/bam/configuration/applier/meta_service.hh"
#include "com/centreon/broker/bam/metric_book.hh"
#include "com/centreon/broker/logging/logging.hh"

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
applier::meta_service::meta_service(
                         applier::meta_service const& other) {
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
void applier::meta_service::apply(
                              state::meta_services const& my_meta,
                              metric_book& book) {
  //
  // DIFF
  //

  // Objects to delete are items remaining in the
  // set at the end of the iteration.
  std::map<unsigned int, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  state::meta_services to_create(my_meta);

  // Objects to modify are items found but
  // with mismatching configuration.
  std::list<configuration::meta_service> to_modify;

  // Iterate through configuration.
  for (state::meta_services::iterator
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
    logging::config(logging::medium)
      << "BAM: removing meta-service " << it->second.cfg.get_id();
    book.unlisten(it->second.cfg.get_id(), it->second.obj.data());
    _applied.erase(it->first);
  }
  to_delete.clear();

  // Create new objects.
  for (state::meta_services::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    logging::config(logging::medium)
      << "BAM: creating meta-service " << it->get_id();
    misc::shared_ptr<bam::meta_service> new_meta(_new_meta(*it, book));
    applied& content(_applied[it->get_id()]);
    content.cfg = *it;
    content.obj = new_meta;
  }

  // Modify existing objects.
  for (std::list<configuration::meta_service>::iterator
         it(to_modify.begin()),
         end(to_modify.end());
       it != end;
       ++it) {
    std::map<unsigned int, applied>::iterator
      pos(_applied.find(it->get_id()));
    if (pos != _applied.end()) {
      logging::config(logging::medium)
        << "BAM: modifying meta-service " << it->get_id();
      _modify_meta(*pos->second.obj, book, pos->second.cfg, *it);
      pos->second.cfg = *it;
    }
    else
      logging::error(logging::high)
        << "BAM: attempting to modify meta-service " << it->get_id()
        << ", however associated object was not found. This is likely a"
        << " software bug that you should report to Centreon Broker "
        << "developers";
  }

  return ;
}

/**
 *  Find a meta-service by its ID.
 *
 *  @param[in] id  Meta-service ID.
 *
 *  @return Shared pointer to the applied meta-service object.
 */
misc::shared_ptr<bam::meta_service> applier::meta_service::find_meta(
                                                             unsigned int id) {
  std::map<unsigned int, applied>::iterator
    it(_applied.find(id));
  return ((it != _applied.end())
          ? it->second.obj
          : misc::shared_ptr<bam::meta_service>());
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void applier::meta_service::_internal_copy(
                              applier::meta_service const& other) {
  _applied = other._applied;
  return ;
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
       it != end;
       ++it) {
    logging::config(logging::low)
      << "BAM: meta-service " << obj.get_id()
      << " does not depend of metric " << *it << " anymore";
    book.unlisten(*it, &obj);
    obj.remove_metric(*it);
  }

  // Add new metrics to 1) the book and to 2) the meta-service.
  for (configuration::meta_service::metric_container::const_iterator
         it(new_cfg.get_metrics().begin()),
         end(new_cfg.get_metrics().end());
       it != end;
       ++it) {
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
  obj.set_level_warning(new_cfg.get_level_warning());
  obj.set_level_critical(new_cfg.get_level_critical());

  return ;
}

/**
 *  Create a new meta-service.
 *
 *  @param[in]  cfg   Meta-service configuration.
 *  @param[out] book  Metric listener book.
 */
misc::shared_ptr<bam::meta_service> applier::meta_service::_new_meta(
                                                             configuration::meta_service const& cfg,
                                                             metric_book& book) {
  misc::shared_ptr<bam::meta_service> meta(new bam::meta_service);
  _modify_meta(*meta, book, configuration::meta_service(), cfg);
  return (meta);
}
