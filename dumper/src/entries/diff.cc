/*
** Copyright 2015 Merethis
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
** <http://www.gnu.org/licenses/>.
*/

#include <map>
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/diff.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/state.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
diff::diff() {}

/**
 *  Diff constructor.
 *
 *  @param[in] older  First state.
 *  @param[in] newer  Second state.
 */
diff::diff(state const& older, state const& newer) {
  // Put objects in more usable containers.
  std::map<unsigned int, ba> old_bas;
  for (std::list<ba>::const_iterator
         it(older.get_bas().begin()),
         end(older.get_bas().end());
       it != end;
       ++it)
    old_bas[it->ba_id] = *it;
  std::map<unsigned int, ba> new_bas;
  for (std::list<ba>::const_iterator
         it(newer.get_bas().begin()),
         end(newer.get_bas().end());
       it != end;
       ++it)
    new_bas[it->ba_id] = *it;
  std::map<unsigned int, kpi> old_kpis;
  for (std::list<kpi>::const_iterator
         it(older.get_kpis().begin()),
         end(older.get_kpis().end());
       it != end;
       ++it)
    old_kpis[it->kpi_id] = *it;
  std::map<unsigned int, kpi> new_kpis;
  for (std::list<kpi>::const_iterator
         it(newer.get_kpis().begin()),
         end(newer.get_kpis().end());
       it != end;
       ++it)
    new_kpis[it->kpi_id] = *it;

  // Diff BAs.
  for (std::map<unsigned int, ba>::const_iterator
         it_old(old_bas.begin()),
         end_old(old_bas.end()),
         it_new(new_bas.begin()),
         end_new(new_bas.end());
       (it_old != end_old) || (it_new != end_new);) {
    if (it_old != end_old) {
      if (it_new != end_new) {
        if (it_old->first == it_new->first) {
          // BA was updated.
          if (it_old->second != it_new->second)
            _bas_to_update.push_back(it_old->second);
          // BA was not modified, nothing to do.

          ++it_old;
          ++it_new;
        }
        // Old BA was deleted.
        else if (it_old->first < it_new->first) {
          _bas_to_delete.push_back(it_old->second);
          _bas_to_delete.back().enable = false;
          ++it_old;
        }
        // New BA was created.
        else {
          _bas_to_create.push_back(it_new->second);
          ++it_new;
        }
      }
      // No more new BAs, old BAs are to be deleted.
      else {
        _bas_to_delete.push_back(it_old->second);
        _bas_to_delete.back().enable = false;
        ++it_old;
      }
    }
    // No more old BAs, new BAs are to be created.
    else {
      _bas_to_create.push_back(it_new->second);
      ++it_new;
    }
  }

  // Diff KPIs.
  for (std::map<unsigned int, kpi>::const_iterator
         it_old(old_kpis.begin()),
         end_old(old_kpis.end()),
         it_new(new_kpis.begin()),
         end_new(new_kpis.end());
       (it_old != end_old) || (it_new != end_new);) {
    if (it_old != end_old) {
      if (it_new != end_new) {
        if (it_old->first == it_new->first) {
          // KPI was updated.
          if (it_old->second != it_new->second)
            _kpis_to_update.push_back(it_old->second);
          // KPI was not modified, nothing to do.

          ++it_old;
          ++it_new;
        }
        // Old KPI was deleted.
        else if (it_old->first < it_new->first) {
          _kpis_to_delete.push_back(it_old->second);
          _kpis_to_delete.back().enable = false;
          ++it_old;
        }
        // New KPI was created.
        else {
          _kpis_to_create.push_back(it_new->second);
          ++it_new;
        }
      }
      // No more new KPIs, old KPIs are to be deleted.
      else {
        _kpis_to_delete.push_back(it_old->second);
        _kpis_to_delete.back().enable = false;
        ++it_old;
      }
    }
    // No more old KPIs, new KPIs are to be created.
    else {
      _kpis_to_create.push_back(it_new->second);
      ++it_new;
    }
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
diff::diff(diff const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
diff::~diff() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
diff& diff::operator=(diff const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Get BAs that should be created.
 *
 *  @return List of BAs that should be created.
 */
std::list<ba> const& diff::bas_to_create() const {
  return (_bas_to_create);
}

/**
 *  Get BAs that should be updated.
 *
 *  @return List of BAs that should be updated.
 */
std::list<ba> const& diff::bas_to_update() const {
  return (_bas_to_update);
}

/**
 *  Get BAs that should be deleted.
 *
 *  @return List of BAs that should be deleted.
 */
std::list<ba> const& diff::bas_to_delete() const {
  return (_bas_to_delete);
}

/**
 *  Get KPIs that should be created.
 *
 *  @return List of KPIs that should be created.
 */
std::list<kpi> const& diff::kpis_to_create() const {
  return (_kpis_to_create);
}

/**
 *  Get KPIs that should be updated.
 *
 *  @return List of KPIs that should be updated.
 */
std::list<kpi> const& diff::kpis_to_update() const {
  return (_kpis_to_update);
}

/**
 *  Get KPIs that should be deleted.
 *
 *  @return List of KPIs that should be deleted.
 */
std::list<kpi> const& diff::kpis_to_delete() const {
  return (_kpis_to_delete);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void diff::_internal_copy(diff const& other) {
  _bas_to_create = other._bas_to_create;
  _bas_to_update = other._bas_to_update;
  _bas_to_delete = other._bas_to_delete;
  _kpis_to_create = other._kpis_to_create;
  _kpis_to_update = other._kpis_to_update;
  _kpis_to_delete = other._kpis_to_delete;
  return ;
}
