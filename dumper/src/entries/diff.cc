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
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/diff.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/state.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

// Place a list of items in a map ordered by ID.
template <typename T, unsigned int (T::* t) >
static void to_map(std::map<unsigned int, T>& m, std::list<T> const& l) {
  for (typename std::list<T>::const_iterator it(l.begin()), end(l.end());
       it != end;
       ++it)
    m[(*it).*t] = *it;
  return ;
}

// Diff two maps and store results in lists.
template <typename T>
static void diff_it(
              std::list<T>& to_create,
              std::list<T>& to_update,
              std::list<T>& to_delete,
              std::map<unsigned int, T> const& old_items,
              std::map<unsigned int, T> const& new_items) {
  to_create.clear();
  to_update.clear();
  to_delete.clear();
  for (typename std::map<unsigned int, T>::const_iterator
         it_old(old_items.begin()),
         end_old(old_items.end()),
         it_new(new_items.begin()),
         end_new(new_items.end());
       (it_old != end_old) || (it_new != end_new);) {
    if (it_old != end_old) {
      if (it_new != end_new) {
        if (it_old->first == it_new->first) {
          // Item was updated.
          if (it_old->second != it_new->second)
            to_update.push_back(it_new->second);
          // Item was not modified, nothing to do.

          ++it_old;
          ++it_new;
        }
        // Old item was deleted.
        else if (it_old->first < it_new->first) {
          to_delete.push_back(it_old->second);
          to_delete.back().enable = false;
          ++it_old;
        }
        // New item was created.
        else {
          to_create.push_back(it_new->second);
          ++it_new;
        }
      }
      // No more new items, old items are to be deleted.
      else {
        to_delete.push_back(it_old->second);
        to_delete.back().enable = false;
        ++it_old;
      }
    }
    // No more old items, new items are to be created.
    else {
      to_create.push_back(it_new->second);
      ++it_new;
    }
  }
  return ;
}

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
  // Diff BA types.
  {
    std::map<unsigned int, ba_type> old_ba_types;
    to_map<ba_type, &ba_type::ba_type_id>(
      old_ba_types,
      older.get_ba_types());
    std::map<unsigned int, ba_type> new_ba_types;
    to_map<ba_type, &ba_type::ba_type_id>(
      new_ba_types,
      newer.get_ba_types());
    diff_it(
      _ba_types_to_create,
      _ba_types_to_update,
      _ba_types_to_delete,
      old_ba_types,
      new_ba_types);
  }

  // Diff BAs.
  {
    std::map<unsigned int, ba> old_bas;
    to_map<ba, &ba::ba_id>(old_bas, older.get_bas());
    std::map<unsigned int, ba> new_bas;
    to_map<ba, &ba::ba_id>(new_bas, newer.get_bas());
    diff_it(
      _bas_to_create,
      _bas_to_update,
      _bas_to_delete,
      old_bas,
      new_bas);
  }

  // Diff KPIs.
  {
    std::map<unsigned int, kpi> old_kpis;
    to_map<kpi, &kpi::kpi_id>(old_kpis, older.get_kpis());
    std::map<unsigned int, kpi> new_kpis;
    to_map<kpi, &kpi::kpi_id>(new_kpis, newer.get_kpis());
    diff_it(
      _kpis_to_create,
      _kpis_to_update,
      _kpis_to_delete,
      old_kpis,
      new_kpis);
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
 *  Get BA types that should be created.
 *
 *  @return List of BA types that should be created.
 */
std::list<ba_type> const& diff::ba_types_to_create() const {
  return (_ba_types_to_create);
}

/**
 *  Get BA types that should be updated.
 *
 *  @return List of BA types that should be updated.
 */
std::list<ba_type> const& diff::ba_types_to_update() const {
  return (_ba_types_to_update);
}

/**
 *  Get BA types that should be deleted.
 *
 *  @return List of BA types that should be deleted.
 */
std::list<ba_type> const& diff::ba_types_to_delete() const {
  return (_ba_types_to_delete);
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
  _ba_types_to_create = other._ba_types_to_create;
  _ba_types_to_update = other._ba_types_to_update;
  _ba_types_to_delete = other._ba_types_to_delete;
  _bas_to_create = other._bas_to_create;
  _bas_to_update = other._bas_to_update;
  _bas_to_delete = other._bas_to_delete;
  _kpis_to_create = other._kpis_to_create;
  _kpis_to_update = other._kpis_to_update;
  _kpis_to_delete = other._kpis_to_delete;
  return ;
}
