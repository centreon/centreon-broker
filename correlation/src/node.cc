/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/node.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

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
    in_downtime(false),
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
node::~node() {
  // Iterators.
  QList<node*>::iterator it, end;

  // Remove self from children.
  for (it = _children.begin(), end = _children.end();
       it != end;
       ++it)
    (*it)->_parents.removeAll(this);

  // Remove self from node depending on self.
  for (it = _depended_by.begin(), end = _depended_by.end();
       it != end;
       ++it)
    (*it)->_depends_on.removeAll(this);

  // Remove self from dependencies.
  for (it = _depends_on.begin(), end = _depends_on.end();
       it != end;
       ++it)
    (*it)->_depended_by.removeAll(this);

  // Remove self from parents.
  for (it = _parents.begin(), end = _parents.end();
       it != end;
       ++it)
    (*it)->_children.removeAll(this);
}

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

/**
 *  Check equality between two nodes.
 *
 *  @param[in] n Node to compare to.
 *
 *  @return true if both nodes are equal.
 */
bool node::operator==(node const& n) const {
  bool retval;
  if (this == &n)
    retval = true;
  else if ((host_id == n.host_id)
           && (in_downtime == n.in_downtime)
           && (service_id == n.service_id)
           && (since == n.since)
           && (state == n.state)
           && ((!my_issue.get() && !n.my_issue.get())
               || (my_issue.get()
                   && n.my_issue.get()
                   && (*my_issue == *n.my_issue)))
           && (_children.size() == n._children.size())
           && (_depended_by.size() == n._depended_by.size())
           && (_depends_on.size() == n._depends_on.size())
           && (_parents.size() == n._parents.size())) {
    retval = true;
    for (QList<node*>::const_iterator
           it1 = _children.begin(),
           end1 = _children.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (QList<node*>::const_iterator
             it2 = n._children.begin(),
             end2 = n._children.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
    for (QList<node*>::const_iterator
           it1 = _depended_by.begin(),
           end1 = _depended_by.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (QList<node*>::const_iterator
             it2 = n._depended_by.begin(),
             end2 = n._depended_by.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
    for (QList<node*>::const_iterator
           it1 = _depends_on.begin(),
           end1 = _depends_on.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (QList<node*>::const_iterator
             it2 = n._depends_on.begin(),
             end2 = n._depends_on.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
    for (QList<node*>::const_iterator
           it1 = _parents.begin(),
           end1 = _parents.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (QList<node*>::const_iterator
             it2 = n._parents.begin(),
             end2 = n._parents.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
  }
  else
    retval = false;
  return (retval);
}

/**
 *  Check inequality between two nodes.
 *
 *  @param[in] n Node to compare to.
 *
 *  @return true if both nodes are not equal.
 */
bool node::operator!=(node const& n) const {
  return (!this->operator==(n));
}

/**
 *  Add a child to the node.
 *
 *  @param[in,out] n New child.
 */
void node::add_child(node* n) {
  _children.push_back(n);
  n->_parents.push_back(this);
  return ;
}

/**
 *  Add a node which depends on this node.
 *
 *  @param[in,out] n New node depending on this node.
 */
void node::add_depended(node* n) {
  _depended_by.push_back(n);
  n->_depends_on.push_back(this);
  return ;
}

/**
 *  Add a dependency.
 *
 *  @param[in,out] n New dependency.
 */
void node::add_dependency(node* n) {
  _depends_on.push_back(n);
  n->_depended_by.push_back(this);
  return ;
}

/**
 *  Add a parent.
 *
 *  @param[in,out] n New parent.
 */
void node::add_parent(node* n) {
  _parents.push_back(n);
  n->_children.push_back(this);
  return ;
}

/**
 *  Get children list.
 *
 *  @return Children list.
 */
QList<node*> const& node::children() const throw () {
  return (_children);
}

/**
 *  Get the list of node which depends on this node.
 *
 *  @return List of node which depends on this node.
 */
QList<node*> const& node::depended_by() const throw () {
  return (_depended_by);
}

/**
 *  Get the list of node on which this node depends.
 *
 *  @return List of node on which this node depends.
 */
QList<node*> const& node::depends_on() const throw () {
  return (_depends_on);
}

/**
 *  Get the list of parent nodes.
 *
 *  @return List of parent nodes.
 */
QList<node*> const& node::parents() const throw () {
  return (_parents);
}

/**
 *  Remove a child node.
 *
 *  @param[in,out] n Child node.
 */
void node::remove_child(node* n) {
  _children.removeAll(n);
  n->_parents.removeAll(this);
  return ;
}

/**
 *  Remove a node which depends on this node.
 *
 *  @param[in,out] n Node which depends on this node.
 */
void node::remove_depended(node* n) {
  _depended_by.removeAll(n);
  n->_depends_on.removeAll(this);
  return ;
}

/**
 *  Remove a node on which this node depends.
 *
 *  @param[in,out] n Node of which this node depends.
 */
void node::remove_dependency(node* n) {
  _depends_on.removeAll(n);
  n->_depended_by.removeAll(this);
  return ;
}

/**
 *  Remove a parent node.
 *
 *  @param[in,out] n Parent node.
 */
void node::remove_parent(node* n) {
  _parents.removeAll(n);
  n->_children.removeAll(this);
  return ;
}

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
  QList<node*>::iterator it, end;

  // Copy childrens.
  _children = n._children;
  for (it = _children.begin(), end = _children.end();
       it != end;
       ++it)
    (*it)->_parents.push_back(this);

  // Copy nodes depending on copied node.
  _depended_by = n._depended_by;
  for (it = _depended_by.begin(), end = _depended_by.end();
       it != end;
       ++it)
    (*it)->_depends_on.push_back(this);

  // Copy nodes on which the copied node depends.
  _depends_on = n._depends_on;
  for (it = _depends_on.begin(), end = _depends_on.end();
       it != end;
       ++it)
    (*it)->_depended_by.push_back(this);

  // Copy parents.
  _parents = n._parents;
  for (it = _parents.begin(), end = _parents.end();
       it != end;
       ++it)
    (*it)->_children.push_back(this);

  // Copy other members.
  host_id = n.host_id;
  in_downtime = n.in_downtime;
  if (n.my_issue.get())
    my_issue.reset(new issue(*(n.my_issue)));
  else
    my_issue.reset();
  service_id = n.service_id;
  since = n.since;
  state = n.state;

  return ;
}
