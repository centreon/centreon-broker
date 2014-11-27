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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/builders/contactgroup_by_contact_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 *
 *  @param[in] group_by_contact  A map of group by contact.
 *  @param[in] contact_by_group  A map of contact by groups.
 */
contactgroup_by_contact_builder::contactgroup_by_contact_builder(
    QMultiHash<unsigned int,objects::contactgroup::ptr>& group_by_contact,
    QMultiHash<objects::contactgroup::ptr, unsigned int>& contact_by_group)
  : _group_by_contact(group_by_contact),
    _contact_by_group(contact_by_group) {}

/**
*  Add a nodegroup to the builder.
*
*  @param[in] id   The nodegroup id.
*  @param[in] ndg  The nodegroup.
*/
void contactgroup_by_contact_builder::add_contactgroup(
                                  unsigned int id,
                                  objects::contactgroup::ptr ctg) {
  _cache.insert(id, ctg);
}

void contactgroup_by_contact_builder::add_contactgroup_contact_relation(
                                        unsigned int contact_id,
                                        unsigned int contactgroup_id) {
  if (_cache.contains(contactgroup_id)) {
    _group_by_contact.insert(contact_id, _cache[contactgroup_id]);
    _contact_by_group.insert(_cache[contactgroup_id], contact_id);
  }
}
