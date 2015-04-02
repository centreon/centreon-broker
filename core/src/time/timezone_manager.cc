/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <ctime>
#include "com/centreon/broker/time/timezone_manager.hh"

using namespace com::centreon::broker::time;

// Class instance.
timezone_manager* timezone_manager::_instance(NULL);

/**
 *  Load singleton.
 */
void timezone_manager::load() {
  if (!_instance)
    _instance = new timezone_manager;
  return ;
}

/**
 *  Lock the timezone manager.
 */
void timezone_manager::lock() {
  _timezone_manager_mutex.lock();
}

/**
 *  Restore timezone previously saved.
 */
void timezone_manager::pop_timezone() {
  // No more timezone available equals no change
  // (base timezone has already been applied).
  if (!_tz.empty()) {
    // Pop timezone.
    tz_info current_tz(_tz.top());
    _tz.pop();
    // Set new timezone.
    _set_timezone(current_tz, _tz.empty() ? _base : _tz.top());
  }
  return ;
}

/**
 *  Save current timezone and set new one.
 *
 *  @param[in] tz  New timezone.
 */
void timezone_manager::push_timezone(char const* tz) {
  // Get timezone info.
  tz_info info;
  _fill_tz_info(&info, tz);

  // Set new timezone.
  _set_timezone(_tz.empty() ? _base : _tz.top(), info);

  // Backup timezone.
  _tz.push(info);

  return ;
}

/**
 *  Unlock the timezone manager.
 */
void timezone_manager::unlock() {
  _timezone_manager_mutex.unlock();
  return ;
}

/**
 *  Unload singleton.
 */
void timezone_manager::unload() {
  delete _instance;
  _instance = NULL;
  return ;
}

/**
 *  Default constructor.
 */
timezone_manager::timezone_manager()
  : _timezone_manager_mutex(QMutex::Recursive) {
  char* base_tz(getenv("TZ"));
  _fill_tz_info(&_base, base_tz);
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
timezone_manager::timezone_manager(timezone_manager const& other)
  : _base(other._base),
    _tz(other._tz),
    _timezone_manager_mutex(QMutex::Recursive) {}

/**
 *  Destructor.
 */
timezone_manager::~timezone_manager() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
timezone_manager& timezone_manager::operator=(
                                      timezone_manager const& other) {
  if (this != &other) {
    _base = other._base;
    _tz = other._tz;
  }
  return (*this);
}

/**
 *  Backup current timezone.
 *
 *  @param[out] info    Timezone information.
 *  @param[in]  old_tz  Timezone string.
 */
void timezone_manager::_fill_tz_info(
                         timezone_manager::tz_info* info,
                         char const* old_tz) {
  if (old_tz) {
    info->is_set = true;
    info->tz_name = old_tz;
  }
  else
    info->is_set = false;
  return ;
}

/**
 *  Set new timezone.
 *
 *  @param[in] from  Timezone information of the current timezone.
 *  @param[in] to    Timezone information of the new timezone.
 */
void timezone_manager::_set_timezone(
                         timezone_manager::tz_info const& from,
                         timezone_manager::tz_info const& to) {
  // Don't set new timezone if it's the same as the actual one.
  if (!((!from.is_set && !to.is_set)
        || (from.is_set && to.is_set && (from.tz_name == to.tz_name)))) {
    if (to.is_set)
      setenv("TZ", to.tz_name.c_str(), 1);
    else
      unsetenv("TZ");
    tzset();
  }
  return ;
}
