/*
** Copyright 2014 Merethis
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

#include <vector>
#include "com/centreon/broker/bam/computable.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
computable::computable() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
computable::computable(computable const& right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
computable::~computable() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
computable& computable::operator=(computable const& right) {
  if (this != &right)
    _internal_copy(right);
  return (*this);
}

/**
 *  Add a new parent.
 *
 *  @param[in] parent Parent node.
 */
void computable::add_parent(
                   misc::shared_ptr<computable> const& parent) {
  _parents.push_back(parent);
  return ;
}

/**
 *  @brief Propagate the update of a child node.
 *
 *  This method will call the child_has_update() method of the parents
 *  of this node.
 *
 *  @param[out] visitor  Object that will receive events.
 */
void computable::propagate_update(stream* visitor) {
  std::vector<bool> filter;
  filter.resize(_parents.size());
  unsigned int i = 0;
  for (std::list<misc::shared_ptr<computable> >::iterator
         it(_parents.begin()),
         end(_parents.end());
       it != end;
       ++it) {
    filter[i++] = (*it)->child_has_update(this, visitor);
  }
  i = 0;
  for (std::list<misc::shared_ptr<computable> >::iterator
         it(_parents.begin()),
         end(_parents.end());
       it != end;
       ++it)
    if (filter[i++] == true)
      (*it)->propagate_update(visitor);
  return ;
}

/**
 *  Remove a parent.
 *
 *  @param[in] parent Parent node.
 */
void computable::remove_parent(
                   misc::shared_ptr<computable> const& parent) {
  for (std::list<misc::shared_ptr<computable> >::iterator
         it(_parents.begin()),
         end(_parents.end());
       it != end;
       ++it)
    if (it->data() == parent.data()) {
      _parents.erase(it);
      break ;
    }
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void computable::_internal_copy(computable const& right) {
  _parents = right._parents;
  return ;
}
