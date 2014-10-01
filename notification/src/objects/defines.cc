/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/defines.hh"

using namespace com::centreon::broker::notification::objects;

node_state::node_state()
  : value(ok) {}

node_state::node_state(short val)
  : value((state_type)val) {}

node_state::node_state(int val)
  : value((state_type)val) {}

node_state::node_state(node_state const& st)
  : value(st.value) {}

node_state& node_state::operator=(node_state const& st) {
  if (this != &st)
    value = st.value;
  return (*this);
}

node_state::operator short() {
  return ((short)(value));
}

node_state::operator int() {
  return ((int)(value));
}
