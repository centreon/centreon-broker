/*
** Copyright 2009-2011 MERETHIS
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

#include "events/group.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal members of the given object to the current
 *         instance.
 *
 *  This internal method is used to copy data defined inside the group
 *  class from an object to the current instance. This means that no
 *  superclass data are copied. This method is used in the copy
 *  constructor and in the assignment operator.
 *
 *  @param[in] g Object to copy.
 *
 *  @see group(group const&)
 *  @see operator=(group const&)
 */
void group::_internal_copy(group const& g) {
  action_url = g.action_url;
  alias = g.alias;
  instance_id = g.instance_id;
  name = g.name;
  notes = g.notes;
  notes_url = g.notes_url;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
group::group() : instance_id(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the Group object to the current instance.
 *
 *  @param[in] g Object to copy.
 */
group::group(group const& g) : event(g) {
  _internal_copy(g);
}

/**
 *  Destructor.
 */
group::~group() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the group object to the current instance.
 *
 *  @param[in] g Object to copy.
 *
 *  @return This object.
 */
group& group::operator=(group const& g) {
  event::operator=(g);
  _internal_copy(g);
  return (*this);
}
