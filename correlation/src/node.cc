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

#include <ctime>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/correlation/log_issue.hh"

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
    instance_id(0),
    in_downtime(false),
    service_id(0),
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
  node_map::iterator it, end;

  // Remove self from children.
  for (it = _children.begin(), end = _children.end();
       it != end;
       ++it)
    (*it)->_parents.remove(get_id());

  // Remove self from node depending on self.
  for (it = _depended_by.begin(), end = _depended_by.end();
       it != end;
       ++it)
    (*it)->_depends_on.remove(get_id());

  // Remove self from dependencies.
  for (it = _depends_on.begin(), end = _depends_on.end();
       it != end;
       ++it)
    (*it)->_depended_by.remove(get_id());

  // Remove self from parents.
  for (it = _parents.begin(), end = _parents.end();
       it != end;
       ++it)
    (*it)->_children.remove(get_id());
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
           && (instance_id == n.instance_id)
           && (in_downtime == n.in_downtime)
           && (service_id == n.service_id)
           && (state == n.state)
           && (downtimes == n.downtimes)
           && ((!my_issue.get() && !n.my_issue.get())
               || (my_issue.get()
                   && n.my_issue.get()
                   && (*my_issue == *n.my_issue)))
           && ((!my_state.get() && n.my_state.get())
               || (my_state.get()
                   && n.my_state.get()
                   && (*my_state == *n.my_state)))
           && (_children.size() == n._children.size())
           && (_depended_by.size() == n._depended_by.size())
           && (_depends_on.size() == n._depends_on.size())
           && (_parents.size() == n._parents.size())) {
    retval = true;
    for (node_map::const_iterator
           it1 = _children.begin(),
           end1 = _children.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (node_map::const_iterator
             it2 = n._children.begin(),
             end2 = n._children.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
    for (node_map::const_iterator
           it1 = _depended_by.begin(),
           end1 = _depended_by.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (node_map::const_iterator
             it2 = n._depended_by.begin(),
             end2 = n._depended_by.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
    for (node_map::const_iterator
           it1 = _depends_on.begin(),
           end1 = _depends_on.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (node_map::const_iterator
             it2 = n._depends_on.begin(),
             end2 = n._depends_on.end();
           it2 != end2;
           ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id)
                  && ((*it1)->service_id == (*it2)->service_id));
    }
    for (node_map::const_iterator
           it1 = _parents.begin(),
           end1 = _parents.end();
         retval && (it1 != end1);
         ++it1) {
      retval = false;
      for (node_map::const_iterator
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
  if (_parents.contains(n->get_id()))
    throw (exceptions::msg()
           << "correlation: trying to insert node ("
           << n->host_id << ", " << n->service_id << ") as children of node ("
           << n->host_id << ", " << n->service_id
           << "), but this node is already a parent");
  _children.insert(n->get_id(), n);
  n->_parents.insert(get_id(), this);
  return ;
}

/**
 *  Add a node which depends on this node.
 *
 *  @param[in,out] n New node depending on this node.
 */
void node::add_depended(node* n) {
  if (_depends_on.contains(n->get_id()))
    throw (exceptions::msg()
           << "correlation: trying to insert node ("
           << n->host_id << ", " << n->service_id << ") as inverse dependency "
           " of node (" << n->host_id << ", " << n->service_id
           << "), but this node is already a dependency");
  _depended_by.insert(n->get_id(), n);
  n->_depends_on.insert(get_id(), this);
  return ;
}

/**
 *  Add a dependency.
 *
 *  @param[in,out] n New dependency.
 */
void node::add_dependency(node* n) {
  if (_depended_by.contains(n->get_id()))
    throw (exceptions::msg()
           << "correlation: trying to insert node ("
           << n->host_id << ", " << n->service_id << ") as dependency of"
           " node (" << n->host_id << ", " << n->service_id
           << "), but this node is already an inverse dependency");
  _depends_on.insert(n->get_id(), n);
  n->_depended_by.insert(get_id(), this);
  return ;
}

/**
 *  Add a parent.
 *
 *  @param[in,out] n New parent.
 */
void node::add_parent(node* n) {
  if (_children.contains(n->get_id()))
    throw (exceptions::msg()
           << "correlation: trying to insert node ("
           << n->host_id << ", " << n->service_id << ") as parent of node ("
           << n->host_id << ", " << n->service_id
           << "), but this node is already a children");
  _parents.insert(n->get_id(), n);
  n->_children.insert(get_id(), this);
  return ;
}

/**
 *  Get the list of children.
 *
 *  @return  The list of children.
 */
node::node_map const& node::get_children() const {
  return (_children);
}

/**
 *  Get the list of dependeds.
 *
 *  @return  The list of depended.
 */
node::node_map const& node::get_dependeds() const {
  return  (_depends_on);
}

/**
 *  Get the list of dependencies.
 *
 *  @return  The list of dependencies.
 */
node::node_map const& node::get_dependencies() const {
  return (_depended_by);
}

/**
 *  Get the list of parents.
 *
 *  @return  The list of parents.
 */
node::node_map const& node::get_parents() const {
  return (_parents);
}

/**
 *  Remove a child node.
 *
 *  @param[in,out] n Child node.
 */
void node::remove_child(node* n) {
  _children.remove(n->get_id());
  n->_parents.remove(this->get_id());
  return ;
}

/**
 *  Remove a node which depends on this node.
 *
 *  @param[in,out] n Node which depends on this node.
 */
void node::remove_depended(node* n) {
  _depended_by.remove(n->get_id());
  n->_depends_on.remove(this->get_id());
  return ;
}

/**
 *  Remove a node on which this node depends.
 *
 *  @param[in,out] n Node of which this node depends.
 */
void node::remove_dependency(node* n) {
  _depends_on.remove(n->get_id());
  n->_depended_by.remove(this->get_id());
  return ;
}

/**
 *  Remove a parent node.
 *
 *  @param[in,out] n Parent node.
 */
void node::remove_parent(node* n) {
  _parents.remove(n->get_id());
  n->_children.remove(this->get_id());
  return ;
}

/**
 *  Get the id of the node.
 *
 *  @return  A pair of host_id, service_id.
 */
QPair<unsigned int, unsigned int> node::get_id() const {
  return (qMakePair(host_id, service_id));
}

/**
 *  Do all the children have issues ?
 *
 *  @return  True if all the children have issues.
 */
bool node::all_children_with_issues() const {
  for (node_map::const_iterator it = _children.begin(), end = _children.end();
       it != end;
       ++it)
    if (!(*it)->my_issue.get())
      return (false);
  return (true);
}

/**
 *  Manage a status event.
 *
 *  @param[in] status            The status.
 *  @param[in] last_state_change The time of the last state change.
 *  @param[out] stream           A stream to write the events to.
 */
void node::manage_status(
       short status,
       timestamp last_state_change,
       io::stream* stream) {

  // No status change, nothing to do.
  if (status == state)
    return ;

  // Recovery
  if (state != 1 && status == 1) {
    my_issue->end_time = last_state_change;
    if (stream)
      stream->write(misc::make_shared(new issue(*my_issue)));
    my_issue.reset();
  }
  // Problem
  else if (state == 1 && status != 1) {
    my_issue.reset(new issue);
    my_issue->start_time = last_state_change;
    my_issue->host_id = host_id;
    my_issue->service_id = service_id;
    my_issue->instance_id = instance_id;
    if (stream)
      stream->write(misc::make_shared(new issue(*my_issue)));
  }

  state = status;

  // Generate the state event.
  _generate_state_event(last_state_change, stream);

  // Visits the parents, children, and dependencies.
  for (node_map::iterator it = _parents.begin(), end = _parents.end();
       it != end;
       ++it)
    (*it)->linked_node_updated(*this, last_state_change, children, stream);
  for (node_map::iterator it = _children.begin(), end = _children.end();
       it != end;
       ++it)
    (*it)->linked_node_updated(*this, last_state_change, parent, stream);
  for (node_map::iterator it = _depends_on.begin(), end = _depends_on.end();
       it != end;
       ++it)
    (*it)->linked_node_updated(*this, last_state_change, depended_by, stream);
  for (node_map::iterator it = _depended_by.begin(), end = _depended_by.end();
       it != end;
       ++it)
    (*it)->linked_node_updated(*this, last_state_change, depends_on, stream);
}

/**
 *  Manage an ack.
 *
 *  @param[in] entry_time  The time of the entry.
 *  @param[out] stream     A stream to write the events to.
 */
void node::manage_ack(timestamp entry_time, io::stream* stream) {
  if (my_issue.get() && !my_issue->ack_time.is_null()) {
    my_issue->ack_time = entry_time;
    _generate_state_event(entry_time, stream);
  }
}

/**
 *  Manage a downtime.
 *
 *  @param[in] dwn        The downtime.
 *  @param[out] stream    A stream to write the events to.
 */
void node::manage_downtime(
        notification::downtime const& dwn,
       io::stream* stream) {
  downtimes[dwn.internal_id] = dwn;
  in_downtime = true;
  _generate_state_event(dwn.start_time, stream);
}

/**
 *  Manage a downtime removal.
 *
 *  @param[in] id         The downtime id.
 *  @param[out] stream    A stream to write the events to.
 */
void node::manage_downtime_removed(
        unsigned int id,
        io::stream* stream) {
  downtimes.erase(id);
  if (!downtimes.empty())
    in_downtime = false;
  _generate_state_event(::time(NULL), stream);
}


/**
 *  Manage a log.
 *
 *  @param[in] entry    The log.
 *  @param[out] stream  A stream to write the events to.
 */
void node::manage_log(
       neb::log_entry const& entry,
       io::stream* stream) {
  if (my_issue.get() && stream) {
    misc::shared_ptr<log_issue> log(new log_issue);
    log->host_id = host_id;
    log->service_id = service_id;
    log->issue_start_time = my_issue->start_time;
    log->log_ctime = entry.c_time;
    stream->write(log);
  }
}

/**
 *  Notify this node that a linked node has been updated.
 *
 *  @param[in] node        The linked node.
 *  @param[in] start_time  The start time of the update.
 *  @param[in] type        What is the linked node from our viewpoint?
 *  @param[out] stream     A stream to write the events to.
 */
void node::linked_node_updated(
       node& n,
       timestamp start_time,
       link_type type,
       io::stream* stream) {
  // Dependencies.
  if ((type == depended_by || type == depends_on)
        && my_issue.get() && n.my_issue.get()) {
    misc::shared_ptr<issue_parent> ip(new issue_parent);
    node& child_node = (type == depends_on ? n : *this);
    node& parent_node = (type == depends_on ? *this : n);
    ip->child_host_id = child_node.host_id;
    ip->child_service_id = child_node.service_id;
    ip->child_instance_id = child_node.instance_id;
    ip->child_start_time = child_node.my_issue->start_time;
    ip->parent_host_id = parent_node.host_id;
    ip->parent_service_id = parent_node.service_id;
    ip->parent_instance_id = parent_node.instance_id;
    ip->parent_start_time = parent_node.my_issue->start_time;
    ip->start_time = start_time;

    if (stream)
      stream->write(ip);
  }
  // Parenting.
  else if ((type == parent || type == children)
             && my_issue.get() && n.my_issue.get()) {
    node& child_node = (type == parent ? n : *this);
    node& parent_node = (type == parent ? *this : n);
    if (parent_node.all_children_with_issues()) {
      misc::shared_ptr<issue_parent> ip(new issue_parent);
      ip->child_host_id = child_node.host_id;
      ip->child_service_id = child_node.service_id;
      ip->child_instance_id = child_node.instance_id;
      ip->child_start_time = child_node.my_issue->start_time;
      ip->parent_host_id = parent_node.host_id;
      ip->parent_service_id = parent_node.service_id;
      ip->parent_instance_id = parent_node.instance_id;
      ip->parent_start_time = parent_node.my_issue->start_time;
      ip->start_time = start_time;

      if (stream)
        stream->write(ip);
    }
  }
}

/**
 *  Serialize the node.
 *
 *  @param[in] cache  A cache to write the event to.
 */
void node::serialize(persistent_cache& cache) const {
  if (my_issue.get())
    cache.add(misc::make_shared(new issue(*my_issue)));
  if (my_state.get())
    cache.add(misc::make_shared(new correlation::state(*my_state)));
  for (std::map<unsigned int, notification::downtime>::const_iterator
         it = downtimes.begin(),
         end = downtimes.end();
       it != end;
       ++it)
    cache.add(misc::make_shared(new notification::downtime(it->second)));
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
  node_map::iterator it, end;

  // Copy childrens.
  _children = n._children;
  for (it = _children.begin(), end = _children.end();
       it != end;
       ++it)
    (*it)->_parents.insert(get_id(), this);

  // Copy nodes depending on copied node.
  _depended_by = n._depended_by;
  for (it = _depended_by.begin(), end = _depended_by.end();
       it != end;
       ++it)
    (*it)->_depends_on.insert(get_id(), this);

  // Copy nodes on which the copied node depends.
  _depends_on = n._depends_on;
  for (it = _depends_on.begin(), end = _depends_on.end();
       it != end;
       ++it)
    (*it)->_depended_by.insert(get_id(), this);

  // Copy parents.
  _parents = n._parents;
  for (it = _parents.begin(), end = _parents.end();
       it != end;
       ++it)
    (*it)->_children.insert(get_id(), this);

  // Copy other members.
  host_id = n.host_id;
  instance_id = n.instance_id;
  in_downtime = n.in_downtime;
  if (n.my_issue.get())
    my_issue.reset(new issue(*(n.my_issue)));
  else
    my_issue.reset();
  service_id = n.service_id;
  state = n.state;
  downtimes = n.downtimes;

  return ;
}

/**
 *  Generate a state event.
 *
 *  @param[in] start_time  The start time of the new event.
 *  @param[out] stream     A stream to write the event to.
 */
void node::_generate_state_event(
       timestamp start_time,
       io::stream* stream) {
  // Close old state event.
  if (my_state.get() && stream) {
    my_state->end_time = start_time;
    stream->write(misc::make_shared(new correlation::state(*my_state)));
  }

  // Open new state event.
  my_state.reset(new correlation::state);
  my_state->start_time = start_time;
  my_state->service_id = service_id;
  my_state->host_id = host_id;
  my_state->current_state = state;
  my_state->instance_id = instance_id;
  timestamp earliest_downtime;
  for (std::map<unsigned int, notification::downtime>::const_iterator
         it = downtimes.begin(),
         end = downtimes.end();
       it != end;
       ++it)
    if (earliest_downtime.is_null()
        || earliest_downtime < it->second.start_time)
      earliest_downtime = it->second.start_time;
  my_state->in_downtime
    = earliest_downtime.is_null() ? false : earliest_downtime <= start_time;
  if (my_issue.get() && !my_issue->ack_time.is_null())
    my_state->ack_time = my_issue->ack_time > start_time ?
                           my_issue->ack_time
                           : start_time;

  if (stream)
    stream->write(misc::make_shared(new correlation::state(*my_state)));
}
