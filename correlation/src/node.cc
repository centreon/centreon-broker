/*
** Copyright 2009-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/correlation/node.hh"
#include <ctime>
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/log_issue.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

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
node::node() {
  // Default state.
  host_id = 0;
  service_id = 0;
  in_downtime = false;
  current_state = 0;
  start_time = time(nullptr);
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
node::node(node const& other) : correlation::state(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
node::~node() {
  // Iterators.
  node_map::iterator it, end;

  // Remove self from children.
  for (it = _children.begin(), end = _children.end(); it != end; ++it)
    (*it)->_parents.erase(this);

  // Remove self from node depending on self.
  for (it = _depended_by.begin(), end = _depended_by.end(); it != end; ++it)
    (*it)->_depends_on.erase(this);

  // Remove self from dependencies.
  for (it = _depends_on.begin(), end = _depends_on.end(); it != end; ++it)
    (*it)->_depended_by.erase(this);

  // Remove self from parents.
  for (it = _parents.begin(), end = _parents.end(); it != end; ++it)
    (*it)->_children.erase(this);
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
node& node::operator=(node const& other) {
  if (this != &other) {
    correlation::state::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Check equality between two nodes.
 *
 *  @param[in] other  Node to compare to.
 *
 *  @return True if both nodes are equal.
 */
bool node::operator==(node const& other) const {
  bool retval;
  if (this == &other)
    retval = true;
  else if (state::operator==(other) && (downtimes == other.downtimes) &&
           ((!my_issue && !other.my_issue) ||
            (my_issue.get() && other.my_issue.get() &&
             (*my_issue == *other.my_issue))) &&
           (_children.size() == other._children.size()) &&
           (_depended_by.size() == other._depended_by.size()) &&
           (_depends_on.size() == other._depends_on.size()) &&
           (_parents.size() == other._parents.size())) {
    retval = true;
    for (node_map::const_iterator it1 = _children.begin(),
                                  end1 = _children.end();
         retval && (it1 != end1); ++it1) {
      retval = false;
      for (node_map::const_iterator it2 = other._children.begin(),
                                    end2 = other._children.end();
           it2 != end2; ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id) &&
                            ((*it1)->service_id == (*it2)->service_id));
    }
    for (node_map::const_iterator it1 = _depended_by.begin(),
                                  end1 = _depended_by.end();
         retval && (it1 != end1); ++it1) {
      retval = false;
      for (node_map::const_iterator it2 = other._depended_by.begin(),
                                    end2 = other._depended_by.end();
           it2 != end2; ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id) &&
                            ((*it1)->service_id == (*it2)->service_id));
    }
    for (node_map::const_iterator it1 = _depends_on.begin(),
                                  end1 = _depends_on.end();
         retval && (it1 != end1); ++it1) {
      retval = false;
      for (node_map::const_iterator it2 = other._depends_on.begin(),
                                    end2 = other._depends_on.end();
           it2 != end2; ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id) &&
                            ((*it1)->service_id == (*it2)->service_id));
    }
    for (node_map::const_iterator it1 = _parents.begin(), end1 = _parents.end();
         retval && (it1 != end1); ++it1) {
      retval = false;
      for (node_map::const_iterator it2 = other._parents.begin(),
                                    end2 = other._parents.end();
           it2 != end2; ++it2)
        retval = retval || (((*it1)->host_id == (*it2)->host_id) &&
                            ((*it1)->service_id == (*it2)->service_id));
    }
  } else
    retval = false;
  return (retval);
}

/**
 *  Check inequality between two nodes.
 *
 *  @param[in] other  Node to compare to.
 *
 *  @return True if both nodes are not equal.
 */
bool node::operator!=(node const& other) const {
  return (!this->operator==(other));
}

/**
 *  Add a child to the node.
 *
 *  @param[in,out] n  New child.
 */
void node::add_child(node* n) {
  if (_parents.find(n) != _parents.end())
    throw(exceptions::msg()
          << "correlation: trying to insert node (" << n->host_id << ", "
          << n->service_id << ") as children of node (" << n->host_id << ", "
          << n->service_id << "), but this node is already a parent");
  _children.insert(n);
  n->_parents.insert(this);
  return;
}

/**
 *  Add a node which depends on this node.
 *
 *  @param[in,out] n  New node depending on this node.
 */
void node::add_depended(node* n) {
  if (_depends_on.find(n) != _depends_on.end())
    throw(exceptions::msg() << "correlation: trying to insert node ("
                            << n->host_id << ", " << n->service_id
                            << ") as inverse dependency "
                               " of node ("
                            << n->host_id << ", " << n->service_id
                            << "), but this node is already a dependency");
  _depended_by.insert(n);
  n->_depends_on.insert(this);
  return;
}

/**
 *  Add a dependency.
 *
 *  @param[in,out] n  New dependency.
 */
void node::add_dependency(node* n) {
  if (_depended_by.find(n) != _depended_by.end())
    throw(exceptions::msg()
          << "correlation: trying to insert node (" << n->host_id << ", "
          << n->service_id
          << ") as dependency of"
             " node ("
          << n->host_id << ", " << n->service_id
          << "), but this node is already an inverse dependency");
  _depends_on.insert(n);
  n->_depended_by.insert(this);
  return;
}

/**
 *  Add a parent.
 *
 *  @param[in,out] n  New parent.
 */
void node::add_parent(node* n) {
  if (_children.find(n) != _children.end())
    throw(exceptions::msg()
          << "correlation: trying to insert node (" << n->host_id << ", "
          << n->service_id << ") as parent of node (" << n->host_id << ", "
          << n->service_id << "), but this node is already a children");
  _parents.insert(n);
  n->_children.insert(this);
  return;
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
  return (_depended_by);
}

/**
 *  Get the list of dependencies.
 *
 *  @return  The list of dependencies.
 */
node::node_map const& node::get_dependencies() const {
  return (_depends_on);
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
  _children.erase(n);
  n->_parents.erase(this);
  return;
}

/**
 *  Remove a node which depends on this node.
 *
 *  @param[in,out] n Node which depends on this node.
 */
void node::remove_depended(node* n) {
  _depended_by.erase(n);
  n->_depends_on.erase(this);
  return;
}

/**
 *  Remove a node on which this node depends.
 *
 *  @param[in,out] n Node of which this node depends.
 */
void node::remove_dependency(node* n) {
  _depends_on.erase(n);
  n->_depended_by.erase(this);
  return;
}

/**
 *  Remove a parent node.
 *
 *  @param[in,out] n Parent node.
 */
void node::remove_parent(node* n) {
  _parents.erase(n);
  n->_children.erase(this);
  return;
}

/**
 *  Get the id of the node.
 *
 *  @return  A pair of host_id, service_id.
 */
std::pair<uint32_t, uint32_t> node::get_id() const {
  return (std::make_pair(host_id, service_id));
}

/**
 *  Do all the parents have issues ?
 *
 *  Additionally, get the maximum start_time of the issue for issue parenting.
 *
 *  @return  True if all the parents have issues.
 */
bool node::all_parents_with_issues_and_get_start_time(
    timestamp& start_time) const {
  // This is a performance bottleneck as it is O(N) where N is
  // the number of parents.
  // A way to gain performance would be to cache this data in all the children.
  for (node_map::const_iterator it = _parents.begin(), end = _parents.end();
       it != end; ++it) {
    if (!(*it)->my_issue)
      return (false);
    if (start_time.is_null() || start_time < (*it)->my_issue->start_time)
      start_time = (*it)->my_issue->start_time;
  }
  return (true);
}

/**
 *  Manage a status event.
 *
 *  @param[in] new_state         The new status.
 *  @param[in] last_state_change The time of the last state change.
 *  @param[out] stream           A stream to write the events to.
 */
void node::manage_status(short new_state,
                         timestamp last_state_change,
                         io::stream* stream) {
  short old_state = current_state;

  // No status change, nothing to do.
  if (old_state == new_state)
    return;

  logging::debug(logging::medium)
      << "correlation: node (" << host_id << ", " << service_id
      << ") changing status from " << old_state << " to " << new_state;

  // Remove acknowledgement.
  if (new_state == 0)
    acknowledgement.reset();
  else if (acknowledgement.get() && !acknowledgement->is_sticky)
    acknowledgement.reset();

  // Generate the state event.
  _generate_state_event(last_state_change, new_state, in_downtime, stream);

  current_state = new_state;

  // Recovery
  if (old_state != 0 && new_state == 0) {
    logging::debug(logging::medium) << "correlation: node (" << host_id << ", "
                                    << service_id << ") closing issue";
    my_issue->end_time = last_state_change;
    _visit_linked_nodes(last_state_change, true, stream);
    _visit_parent_of_child_nodes(last_state_change, true, stream);
    if (stream)
      stream->write(std::make_shared<issue>(*my_issue));
    my_issue.reset();
  }
  // Problem
  else if (old_state == 0 && new_state != 0) {
    logging::debug(logging::medium) << "correlation: node (" << host_id << ", "
                                    << service_id << ") opening issue";
    my_issue.reset(new issue);
    my_issue->start_time = last_state_change;
    my_issue->host_id = host_id;
    my_issue->service_id = service_id;
    if (acknowledgement)
      my_issue->ack_time = last_state_change;
    if (stream)
      stream->write(std::make_shared<issue>(*my_issue));
    _visit_linked_nodes(last_state_change, false, stream);
    _visit_parent_of_child_nodes(last_state_change, false, stream);
  }
}

/**
 *  Manage an ack.
 *
 *  @param[in] ack         The acknowledgement.
 *  @param[out] stream     A stream to write the events to.
 */
void node::manage_ack(neb::acknowledgement const& ack, io::stream* stream) {
  // Acknowledgement was created.
  if (ack.deletion_time.is_null()) {
    logging::debug(logging::low)
        << "correlation: acknowledgement on node (" << ack.host_id << ", "
        << ack.service_id << ") created at " << ack.entry_time;
    acknowledgement.reset(new neb::acknowledgement(ack));

    // Update issue.
    if (my_issue) {
      my_issue->ack_time = ack.entry_time;
      if (stream)
        stream->write(std::make_shared<issue>(*my_issue));
    }

    // Update state event.
    ack_time = ack.entry_time;
    if (stream)
      stream->write(std::make_shared<state>(*this));
  }
  // Acknowledgement was deleted.
  else {
    logging::debug(logging::low)
        << "correlation: acknowledgement on node (" << ack.host_id << ", "
        << ack.service_id << ") created at " << ack.entry_time
        << " was deleted at " << ack.deletion_time;
    acknowledgement.reset();
  }
  return;
}

/**
 *  Manage a downtime.
 *
 *  @param[in] dwn        The downtime.
 *  @param[out] stream    A stream to write the events to.
 */
void node::manage_downtime(neb::downtime const& dwn, io::stream* stream) {
  bool started(!dwn.actual_start_time.is_null());
  bool finished(!dwn.actual_end_time.is_null());
  if (started) {
    if (!finished) {
      logging::debug(logging::low)
          << "correlation: downtime (" << dwn.actual_start_time << "-"
          << dwn.actual_end_time << ") on node (" << dwn.host_id << ", "
          << dwn.service_id << ") is starting";
      downtimes[dwn.internal_id] = dwn;
      if (!in_downtime)
        _generate_state_event(dwn.actual_start_time, current_state, true,
                              stream);
    } else {
      logging::debug(logging::low)
          << "correlation: downtime (" << dwn.actual_start_time << "-"
          << dwn.actual_end_time << ") on node (" << dwn.host_id << ", "
          << dwn.service_id << ") finished";
      downtimes.erase(dwn.internal_id);
      if (downtimes.empty())
        _generate_state_event(dwn.actual_end_time, current_state, false,
                              stream);
    }
  }
  return;
}

/**
 *  Manage a log.
 *
 *  @param[in] entry    The log.
 *  @param[out] stream  A stream to write the events to.
 */
void node::manage_log(neb::log_entry const& entry, io::stream* stream) {
  if (my_issue.get() && stream) {
    std::shared_ptr<log_issue> log(new log_issue);
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
 *  @param[in] closed      Is the event closed?
 *  @param[in] type        What is the linked node from our viewpoint?
 *  @param[out] stream     A stream to write the events to.
 */
void node::linked_node_updated(node& n,
                               timestamp start_time,
                               bool closed,
                               link_type type,
                               io::stream* stream) {
  // Dependencies.
  if ((type == depended_by || type == depends_on) && my_issue.get() &&
      n.my_issue.get()) {
    std::shared_ptr<issue_parent> ip(new issue_parent);
    node& child_node = (type == depended_by ? n : *this);
    node& parent_node = (type == depended_by ? *this : n);
    ip->child_host_id = child_node.host_id;
    ip->child_service_id = child_node.service_id;
    ip->child_start_time = child_node.my_issue->start_time;
    ip->parent_host_id = parent_node.host_id;
    ip->parent_service_id = parent_node.service_id;
    ip->parent_start_time = parent_node.my_issue->start_time;
    ip->start_time = my_issue->start_time > n.my_issue->start_time
                         ? my_issue->start_time
                         : n.my_issue->start_time;
    if (closed)
      ip->end_time = start_time;

    if (stream)
      stream->write(ip);
  }
  // Parenting.
  // We are doing an interesting thing to get the start_time of the issue
  // parenting. It is the maximum of all the start_time of the issues of
  // the child and all its parents.
  else if ((type == parent || type == children) && my_issue.get() &&
           n.my_issue.get()) {
    node& child_node = (type == parent ? *this : n);
    node& parent_node = (type == parent ? n : *this);
    timestamp start_time_of_the_issue_parenting =
        child_node.my_issue->start_time;
    if (child_node.all_parents_with_issues_and_get_start_time(
            start_time_of_the_issue_parenting)) {
      std::shared_ptr<issue_parent> ip(new issue_parent);
      ip->child_host_id = child_node.host_id;
      ip->child_service_id = child_node.service_id;
      ip->child_start_time = child_node.my_issue->start_time;
      ip->parent_host_id = parent_node.host_id;
      ip->parent_service_id = parent_node.service_id;
      ip->parent_start_time = parent_node.my_issue->start_time;
      ip->start_time = start_time_of_the_issue_parenting;
      if (closed)
        ip->end_time = start_time;

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
  if (my_issue)
    cache.add(std::make_shared<issue>(*my_issue));
  cache.add(std::make_shared<correlation::state>(*this));
  for (std::map<uint32_t, neb::downtime>::const_iterator
           it = downtimes.begin(),
           end = downtimes.end();
       it != end; ++it)
    cache.add(std::make_shared<neb::downtime>(it->second));
  if (acknowledgement)
    cache.add(std::make_shared<neb::acknowledgement>(*acknowledgement));
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
  // Copy other members.
  if (n.my_issue)
    my_issue.reset(new issue(*(n.my_issue)));
  else
    my_issue.reset();
  downtimes = n.downtimes;
  if (n.acknowledgement)
    acknowledgement.reset(new neb::acknowledgement(*n.acknowledgement));
  else
    acknowledgement.reset();

  // Copy childrens.
  node_map::iterator it, end;
  _children = n._children;
  for (it = _children.begin(), end = _children.end(); it != end; ++it)
    (*it)->_parents.insert(this);

  // Copy nodes depending on copied node.
  _depended_by = n._depended_by;
  for (it = _depended_by.begin(), end = _depended_by.end(); it != end; ++it)
    (*it)->_depends_on.insert(this);

  // Copy nodes on which the copied node depends.
  _depends_on = n._depends_on;
  for (it = _depends_on.begin(), end = _depends_on.end(); it != end; ++it)
    (*it)->_depended_by.insert(this);

  // Copy parents.
  _parents = n._parents;
  for (it = _parents.begin(), end = _parents.end(); it != end; ++it)
    (*it)->_children.insert(this);

  return;
}

/**
 *  Generate a state event.
 *
 *  @param[in]  start_time       The start time of the new event.
 *  @param[in]  new_status       The status of the new event.
 *  @param[in]  new_in_downtime  The in_downtime flag of the new event.
 *  @param[out] stream           A stream to write the event to.
 */
void node::_generate_state_event(timestamp start_time,
                                 short new_status,
                                 bool new_in_downtime,
                                 io::stream* stream) {
  // Close old state event.
  if (stream) {
    logging::debug(logging::medium) << "correlation: node (" << host_id << ", "
                                    << service_id << ") closing state event";
    end_time = start_time;
    stream->write(std::make_shared<correlation::state>(*this));
  }

  // Open new state event.
  logging::debug(logging::medium) << "correlation: node (" << host_id << ", "
                                  << service_id << ") opening new state event";
  if (acknowledgement.get() &&
      !acknowledgement->is_sticky
      // Downtime start/stop do not remove non-sticky acknowledgements.
      && (in_downtime == new_in_downtime)) {
    logging::debug(logging::low)
        << "correlation: reseting non-sticky acknowledgement of node ("
        << host_id << ", " << service_id << ")";
    acknowledgement.reset();
  }
  *static_cast<correlation::state*>(this) = _open_state_event(start_time);
  current_state = new_status;
  in_downtime = new_in_downtime;
  if (stream)
    stream->write(std::make_shared<correlation::state>(*this));
  return;
}

/**
 *  Open a new state event.
 *
 *  @param[in] start_time  The start time of the event.
 *
 *  @return                A new state event.
 */
correlation::state node::_open_state_event(timestamp start_time) const {
  correlation::state st;
  st.start_time = start_time;
  st.service_id = service_id;
  st.host_id = host_id;
  st.current_state = current_state;
  timestamp earliest_downtime;
  for (std::map<uint32_t, neb::downtime>::const_iterator
           it(downtimes.begin()),
       end(downtimes.end());
       it != end; ++it)
    if (earliest_downtime.is_null() ||
        earliest_downtime > it->second.start_time)
      earliest_downtime = it->second.start_time;
  st.in_downtime =
      earliest_downtime.is_null() ? false : earliest_downtime <= start_time;
  if (acknowledgement)
    st.ack_time = acknowledgement->entry_time > start_time
                      ? acknowledgement->entry_time
                      : start_time;
  return (st);
}

/**
 *  Visits the parents, children, and dependencies.
 *
 *  @param[in] last_state_change   When was the node last updated.
 *  @param[in] closed              Is the event closed?
 *  @param[in] stream              The stream.
 */
void node::_visit_linked_nodes(timestamp last_state_change,
                               bool closed,
                               io::stream* stream) {
  for (node_map::iterator it = _parents.begin(), end = _parents.end();
       it != end; ++it)
    (*it)->linked_node_updated(*this, last_state_change, closed, children,
                               stream);
  for (node_map::iterator it = _children.begin(), end = _children.end();
       it != end; ++it)
    (*it)->linked_node_updated(*this, last_state_change, closed, parent,
                               stream);
  for (node_map::iterator it = _depends_on.begin(), end = _depends_on.end();
       it != end; ++it)
    (*it)->linked_node_updated(*this, last_state_change, closed, depended_by,
                               stream);
  for (node_map::iterator it = _depended_by.begin(), end = _depended_by.end();
       it != end; ++it)
    (*it)->linked_node_updated(*this, last_state_change, closed, depends_on,
                               stream);
}

/**
 *  Visit the parents of the children to notify that a parenting state
 *  was changed.
 *
 *  @param[in] last_state_change  The change of the last state.
 *  @param[in] closed             Is the event closed?
 *  @param[out] stream            The stream.
 */
void node::_visit_parent_of_child_nodes(timestamp last_state_change,
                                        bool closed,
                                        io::stream* stream) {
  for (node_map::iterator it = _children.begin(), end = _children.end();
       it != end; ++it) {
    for (node_map::iterator it2 = (*it)->_parents.begin(),
                            end2 = (*it)->_parents.end();
         it2 != end2; ++it2) {
      if (*it2 != this)
        (*it2)->linked_node_updated(**it, last_state_change, closed, children,
                                    stream);
    }
  }
}
