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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::ba::ba() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
applier::ba::ba(applier::ba const& right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
applier::ba::~ba() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
applier::ba& applier::ba::operator=(applier::ba const& right) {
  if (this != &right)
    _internal_copy(right);
  return (*this);
}

/**
 *  Apply configuration.
 *
 *  @param[in] my_bas BAs to apply.
 *  @param[in] book   The service book.
 *  @param[in] tp     The timeperiod applier.
 */
void applier::ba::apply(bam::configuration::state::bas const& my_bas,
                        service_book& book,
                        timeperiod& tp) {
  //
  // DIFF
  //

  // Objects to delete are items remaining in the
  // set at the end of the iteration.
  std::map<unsigned int, applied> to_delete(_applied);

  // Objects to create are items remaining in the
  // set at the end of the iteration.
  bam::configuration::state::bas to_create(my_bas);

  // Objects to modify are items found but
  // with mismatching configuration.
  std::list<bam::configuration::ba> to_modify;

  // Iterate through configuration.
  for (bam::configuration::state::bas::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;) {
    std::map<unsigned int, applied>::iterator
      cfg_it(to_delete.find(it->first));
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
  for (std::map<unsigned int, applied>::iterator
         it(to_delete.begin()),
         end(to_delete.end());
       it != end;
       ++it) {
    logging::config(logging::medium)
      << "BAM: removing BA " << it->first;
    book.unlisten(
           it->second.cfg.get_host_id(),
           it->second.cfg.get_service_id(),
           static_cast<bam::ba*>(it->second.obj.data()));
    _applied.erase(it->first);
  }
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::bas::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    logging::config(logging::medium)
      << "BAM: creating BA " << it->first;
    misc::shared_ptr<bam::ba> new_ba(_new_ba(it->second, book, tp));
    applied& content(_applied[it->first]);
    content.cfg = it->second;
    content.obj = new_ba;
  }

  // Modify existing objects.
  for (std::list<bam::configuration::ba>::iterator
         it(to_modify.begin()),
         end(to_modify.end());
       it != end;
       ++it) {
    std::map<unsigned int, applied>::iterator
      pos(_applied.find(it->get_id()));
    if (pos != _applied.end()) {
      logging::config(logging::medium)
        << "BAM: modifying BA " << it->get_id();
      pos->second.obj->set_level_warning(it->get_warning_level());
      pos->second.obj->set_level_critical(it->get_critical_level());
      pos->second.cfg = *it;
    }
    else
      logging::error(logging::high)
        << "BAM: attempting to modify BA " << it->get_id()
        << ", however associated object was not found. This is likely a"
        << " software bug that you should report to Centreon Broker "
        << "developers";
  }

  return ;
}

/**
 *  Find BA by its ID.
 *
 *  @param[in] id BA ID.
 *
 *  @return Shared pointer to the applied BA object.
 */
misc::shared_ptr<bam::ba> applier::ba::find_ba(unsigned int id) {
  std::map<unsigned int, applied>::iterator
    it(_applied.find(id));
  return ((it != _applied.end())
          ? it->second.obj
          : misc::shared_ptr<bam::ba>());
}

/**
 *  Visit each applied BA.
 *
 *  @param[out] visitor  Visitor that will receive status.
 */
void applier::ba::visit(monitoring_stream* visitor) {
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
void applier::ba::_internal_copy(applier::ba const& right) {
  _applied = right._applied;
  return ;
}

/**
 *  Create new BA object.
 *
 *  @param[in] cfg BA configuration.
 *
 *  @return New BA object.
 */
misc::shared_ptr<bam::ba> applier::ba::_new_ba(
                                         configuration::ba const& cfg,
                                         service_book& book,
                                         timeperiod& tp) {
  misc::shared_ptr<bam::ba> obj(new bam::ba);
  obj->set_id(cfg.get_id());
  obj->set_host_id(cfg.get_host_id());
  obj->set_service_id(cfg.get_service_id());
  obj->set_level_warning(cfg.get_warning_level());
  obj->set_level_critical(cfg.get_critical_level());
  if (cfg.get_opened_event().ba_id)
    obj->set_initial_event(cfg.get_opened_event());
  if (!cfg.get_default_timeperiod())
    throw (exceptions::msg()
           << "BAM: Could not find the default timeperiod for the ba: " << obj->get_id());
  _apply_timeperiods(obj, cfg.get_default_timeperiod(), cfg.get_timeperiods(), tp);
  book.listen(cfg.get_host_id(), cfg.get_service_id(), obj.data());
  return (obj);
}

/**
 *  Apply the timeperiods to a BA object.
 *
 *  @param[out] obj        The object to be applied.
 *  @param[in] default_tp  The default timeperiod id.
 *  @param[in] tps         The other timeperiods id.
 *  @param[in] tp          The timeperiod applier.
 */
void applier::ba::_apply_timeperiods(misc::shared_ptr<bam::ba> obj,
                                     unsigned int default_tp,
                                     std::vector<unsigned int> const& tps,
                                     timeperiod& tp) {
  obj->clear_timeperiods();

  // Set the default timeperiod.
  {
    time::timeperiod::ptr resolved_tp = tp.get_timeperiod_by_id(default_tp);
    if (resolved_tp.isNull())
      throw (exceptions::msg()
        << "BAM: Could not find the timeperiod " << default_tp
        << " for the ba: " << obj->get_id());
    obj->add_timeperiod(resolved_tp, true);
  }

  // Set the other timeperiods.
  for (std::vector<unsigned int>::const_iterator it(tps.begin()),
                                                 end(tps.end());
       it != end;
       ++it) {
    time::timeperiod::ptr resolved_tp = tp.get_timeperiod_by_id(*it);
    if (resolved_tp.isNull())
      throw (exceptions::msg()
        << "BAM: Could not find the timeperiod " << *it
        << " for the ba: " << obj->get_id());
     obj->add_timeperiod(resolved_tp, false);
  }
}
