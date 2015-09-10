/*
** Copyright 2015 Centreon
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

#include <map>
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/diff.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
#include "com/centreon/broker/dumper/entries/boolean.hh"
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

  // Diff boolean rules.
  {
    std::map<unsigned int, boolean> old_booleans;
    to_map<boolean, &boolean::boolean_id>(old_booleans, older.get_booleans());
    std::map<unsigned int, boolean> new_booleans;
    to_map<boolean, &boolean::boolean_id>(new_booleans, newer.get_booleans());
    diff_it(
      _booleans_to_create,
      _booleans_to_update,
      _booleans_to_delete,
      old_booleans,
      new_booleans);
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

  // Diff hosts.
  {
    std::map<unsigned int, host> old_hosts;
    to_map<host, &host::host_id>(old_hosts, older.get_hosts());
    std::map<unsigned int, host> new_hosts;
    to_map<host, &host::host_id>(new_hosts, newer.get_hosts());
    diff_it(
      _hosts_to_create,
      _hosts_to_update,
      _hosts_to_delete,
      old_hosts,
      new_hosts);
  }

  // Diff services.
  {
    std::map<unsigned int, service> old_services;
    to_map<service, &service::service_id>(old_services, older.get_services());
    std::map<unsigned int, service> new_services;
    to_map<service, &service::service_id>(new_services, newer.get_services());
    diff_it(
      _services_to_create,
      _services_to_update,
      _services_to_delete,
      old_services,
      new_services);
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
 *  Get the boolean rules that should be created.
 *
 *  @return List of boolean rules that should be created.
 */
std::list<boolean> const& diff::booleans_to_create() const {
  return (_booleans_to_create);
}

/**
 *  Get the boolean rules that should be updated.
 *
 *  @return List of boolean rules that should be updated.
 */
std::list<boolean> const& diff::booleans_to_update() const {
  return (_booleans_to_update);
}

/**
 *  Get the boolean rules that should be deleted.
 *
 *  @return List of boolean rules that should be deleted.
 */
std::list<boolean> const& diff::booleans_to_delete() const {
  return (_booleans_to_delete);
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

/**
 *  Get hosts that should be created.
 *
 *  @return List of hosts that should be created.
 */
std::list<host> const& diff::hosts_to_create() const {
  return (_hosts_to_create);
}

/**
 *  Get hosts that should be updated.
 *
 *  @return List of hosts that should be updated.
 */
std::list<host> const& diff::hosts_to_update() const {
  return (_hosts_to_update);
}

/**
 *  Get hosts that should be deleted.
 *
 *  @return List of hosts that should be deleted.
 */
std::list<host> const& diff::hosts_to_delete() const {
  return (_hosts_to_delete);
}

/**
 *  Get services that should be created.
 *
 *  @return List of services that should be created.
 */
std::list<service> const& diff::services_to_create() const {
  return (_services_to_create);
}

/**
 *  Get services that should be updated.
 *
 *  @return List of services that should be updated.
 */
std::list<service> const& diff::services_to_update() const {
  return (_services_to_update);
}

/**
 *  Get services that should be deleted.
 *
 *  @return List of services that should be deleted.
 */
std::list<service> const& diff::services_to_delete() const {
  return (_services_to_delete);
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
  _booleans_to_create = other._booleans_to_create;
  _booleans_to_update = other._booleans_to_update;
  _booleans_to_delete = other._booleans_to_delete;
  _kpis_to_create = other._kpis_to_create;
  _kpis_to_update = other._kpis_to_update;
  _kpis_to_delete = other._kpis_to_delete;
  _hosts_to_create = other._hosts_to_create;
  _hosts_to_update = other._hosts_to_update;
  _hosts_to_delete = other._hosts_to_delete;
  _services_to_create = other._services_to_create;
  _services_to_update = other._services_to_update;
  _services_to_delete = other._services_to_delete;
  return ;
}
