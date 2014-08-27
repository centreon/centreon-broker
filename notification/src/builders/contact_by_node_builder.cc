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

#include "com/centreon/broker/notification/builders/contact_by_node_builder.hh"

using namespace com::centreon::broker::notification;

contact_by_node_builder::contact_by_node_builder(QMultiHash<node_id, contact::ptr>& table) :
  _table(table) {}

void contact_by_node_builder::add_contact(unsigned int id,
                                           contact::ptr con) {
  _cache[id] = con;
}

void contact_by_node_builder::connect_contact_node_id(unsigned int contact_id,
                                                      node_id id) {
  if (!id.has_service())
    _table.insert(id, _cache[contact_id]);
  else if (!id.has_host())
    _table.insert(id, _cache[contact_id]);
  else {
    // This node_id could have been inserted as a stand alone service beforehand.
    // In this case, delete it.
    contact::ptr cont = _cache[contact_id];
    QMultiHash<node_id, contact::ptr>::iterator found = _table.find(id);
    QMultiHash<node_id, contact::ptr>::iterator end = _table.end();
    for (; found != end; ++found)
      if (found.value() == cont) {
        _table.erase(found);
        break;
      }
     _table.insert(id, cont);
  }
}
