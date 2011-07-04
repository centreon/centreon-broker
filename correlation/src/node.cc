/*
** Copyright 2009-2011 Merethis
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

#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/node.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] n Object to copy.
 */
void node::_internal_copy(node const& n) {
  children = n.children;
  depended_by = n.depended_by;
  depends_on = n.depends_on;
  host_id = n.host_id;
  if (!n.my_issue.isNull())
    my_issue = QSharedPointer<issue>(new issue(*(n.my_issue)));
  else
    my_issue.clear();
  parents = n.parents;
  service_id = n.service_id;
  since = n.since;
  state = n.state;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
node::node()
  : host_id(0),
    my_issue(NULL),
    service_id(0),
    since(0),
    state(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] n Object to copy.
 */
node::node(node const& n) {
  _internal_copy(n);
}

/**
 *  Destructor.
 */
node::~node() {}

/**
 *  Assignment operator.
 *
 *  @param[in] n Object to copy.
 *
 *  @return This object.
 */
node& node::operator=(node const& n) {
  _internal_copy(n);
  return (*this);
}
