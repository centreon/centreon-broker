/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/notification/macro_context.hh"

using namespace com::centreon::broker::notification;

macro_context::macro_context(
                 objects::node_id id,
                 objects::contact const& cnt,
                 state const& st,
                 node_cache const& cache)
  : _id(id),
    _cnt(cnt),
    _st(st),
    _cache(cache) {}

objects::node_id macro_context::get_id() const {
  return (_id);
}

objects::contact const& macro_context::get_contact() const {
  return (_cnt);
}

state const& macro_context::get_state() const {
  return (_st);
}

node_cache const& macro_context::get_cache() const {
  return (_cache);
}
