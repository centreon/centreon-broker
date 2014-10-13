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

#include "com/centreon/broker/notification/node_cache.hh"

using namespace com::centreon::broker::notification;

node_cache::node_cache() {}

node_cache::node_cache(node_cache const& obj) {
  node_cache::operator=(obj);
}

node_cache& node_cache::operator=(node_cache const& obj) {
  if (this != &obj) {

  }
  return (*this);
}

bool node_cache::load(std::string const& cache_file) {
  return (true);
}

bool node_cache::unload(std::string const& cache_file) {
  return (true);
}
