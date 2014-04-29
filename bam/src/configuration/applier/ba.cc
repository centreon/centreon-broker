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
 */
void applier::ba::apply(bam::configuration::state::bas const& my_bas) {
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
       ++it)
    _applied.erase(it->first);
  to_delete.clear();

  // Create new objects.
  for (bam::configuration::state::bas::iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    misc::shared_ptr<bam::ba> new_ba(_new_ba(*it));
    applied& content(_applied[it->get_id()]);
    content.cfg = *it;
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
      // XXX : apply modified configuration
    }
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
void applier::ba::visit(stream* visitor) {
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
                                         configuration::ba const& cfg) {
  misc::shared_ptr<bam::ba> obj(new bam::ba);
  obj->set_id(cfg.get_id());
  obj->set_level_warning(cfg.get_warning_level());
  obj->set_level_critical(cfg.get_critical_level());
  return (obj);
}
