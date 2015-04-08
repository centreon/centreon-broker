/*
** Copyright 2009-2014 Merethis
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
#include <QDomDocument>
#include <QDomElement>
#include <unistd.h>
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/neb/instance_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Determine whether or not a node should have the unknown state.
 *
 *  @param[in] n Node to check.
 *
 *  @return true if the node should be unknown.
 */
static bool should_be_unknown(node const& n) {
  bool all_parents_down;
  bool one_dependency_down;

  // If node has no parents, then all_parents_down will be false.
  if (!n.parents().isEmpty()) {
    all_parents_down = true;
    for (QList<node*>::const_iterator
           it(n.parents().begin()),
           end(n.parents().end());
         it != end;
         ++it)
      all_parents_down = (all_parents_down && (*it)->state);
  }
  else
    all_parents_down = false;

  // Check dependencies.
  one_dependency_down = false;
  for (QList<node*>::const_iterator
         it(n.depends_on().begin()),
         end(n.depends_on().end());
       it != end;
       ++it)
    one_dependency_down = (one_dependency_down || (*it)->state);

  // Debug message.
  if (all_parents_down && one_dependency_down)
    logging::debug(logging::medium)
      << "correlation: unknown state of node (" << n.host_id << ", "
      << n.service_id << ") is triggered by parenting AND dependencies";
  else if (all_parents_down)
    logging::debug(logging::medium)
      << "correlation: unknown state of node (" << n.host_id << ", "
      << n.service_id << ") is triggered by parenting";
  else if (one_dependency_down)
    logging::debug(logging::medium)
      << "correlation: unknown state of node (" << n.host_id << ", "
      << n.service_id << ") is triggered by dependencies";

  return (all_parents_down || one_dependency_down);
}

/**
 *  Get the unknown state that match a host.
 */
static int unknown_state(node const& n) {
  return (n.service_id ? 3 : 2); // Unknown is UNREACHABLE for hosts.
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] instance_id The broker instance.
 *  @param[in] is_passive  True if the correlator is on passive mode.
 *                         In passive mode, the correlator only update
 *                         internal state.
 */
correlator::correlator(unsigned int instance_id, bool is_passive)
  : _instance_id(instance_id),
    _process_event(is_passive
                   ? &correlator::_process_event_on_passive
                   : &correlator::_process_event_on_active) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
correlator::correlator(correlator const& c) : multiplexing::hooker(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
correlator::~correlator() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
correlator& correlator::operator=(correlator const& c) {
  multiplexing::hooker::operator=(c);
  _internal_copy(c);
  return (*this);
}

/**
 *  Get the correlation state.
 *
 *  @return Current correlation state.
 */
QMap<QPair<unsigned int, unsigned int>, node> const& correlator::get_state() const {
  return (_nodes);
}

/**
 *  Load a correlation file.
 *
 *  @param[in] correlation_file Path to a file containing host and
 *                              service relationships.
 *  @param[in] retention_file   Path to a file to which correlation
 *                              information will be dumped when the
 *                              correlation engine stops.
 */
void correlator::load(
                   QString const& correlation_file,
                   QString const& retention_file) {
  // Set files.
  _correlation_file = correlation_file;
  _retention_file = retention_file;

  // Load configuration file.
  {
    logging::config(logging::medium)
      << "correlation: loading configuration file";
    parser p;
    p.parse(_correlation_file, false, _nodes);
  }

  // Load retention file.
  if (!_retention_file.isEmpty()
      && !access(_retention_file.toStdString().c_str(), F_OK)) {
    logging::config(logging::medium)
      << "correlation: loading retention file";
    persistent_cache cache(_retention_file.toStdString());
    misc::shared_ptr<io::data> data;
    do {
      cache.get(data);
      if (!data.isNull() && data->type() == issue::static_type()) {
        issue const& is = *data.staticCast<issue>();
        if (!_nodes.contains(qMakePair(is.host_id, is.service_id)))
          throw (
            exceptions::msg()
            << "correlation: couldn't find node (" << is.host_id
            << ", " << is.service_id << ")");
        _nodes.find(qMakePair(is.host_id, is.service_id))->my_issue
                                   = std::auto_ptr<issue>(new issue(is));
      }
    }
    while (!data.isNull());
  }

  // Reopen issues.
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it(_nodes.begin()),
         end(_nodes.end());
       it != end;
       ++it)
    if (it->my_issue.get()) {
      logging::info(logging::medium)
        << "correlation: reopening issue of node (" << it->host_id
        << ", " << it->service_id << "), state " << it->state;
      _events.push_back(
                misc::shared_ptr<io::data>(new issue(*(it->my_issue))));
    }

  // Re-link issues.
  for (QMap<QPair<unsigned int, unsigned int>, node>::iterator
         it(_nodes.begin()),
         end(_nodes.end());
       it != end;
       ++it)
    if (it->my_issue.get())
      _issue_parenting(&*it, false);

  return ;
}

/**
 *  Get the next available correlated event.
 *
 *  @param[out] e The next available correlated event.
 */
void correlator::read(misc::shared_ptr<io::data>& e) {
  e.clear();
  if (!_events.empty()) {
    e = _events.front();
    _events.pop_front();
  }
  return ;
}

/**
 *  Set correlator state.
 *
 *  @param[in] state New correlation state.
 */
void correlator::set_state(QMap<QPair<unsigned int, unsigned int>, node> const& state) {
  // Copy nodes.
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it(state.begin()),
         end(state.end());
       it != end;
       ++it) {
    node& n(_nodes[qMakePair(it->host_id, it->service_id)]);
    n.host_id = it->host_id;
    n.instance_id = it->instance_id;
    n.service_id = it->service_id;
    n.since = it->since;
    n.state = it->state;
    if (it->my_issue.get())
      n.my_issue.reset(new issue(*it->my_issue));
  }

  // Copy node links.
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it(state.begin()),
         end(state.end());
       it != end;
       ++it) {
    node& n(_nodes[qMakePair(it->host_id, it->service_id)]);
    // Copy children.
    for (QList<node*>::const_iterator
           it2(it->children().begin()),
           end2(it->children().end());
         it2 != end2;
         ++it2)
      n.add_child(&_nodes[qMakePair(
        (*it2)->host_id,
        (*it2)->service_id)]);
    // Copy dependencies.
    for (QList<node*>::const_iterator
           it2(it->depended_by().begin()),
           end2(it->depended_by().end());
         it2 != end2;
         ++it2)
      n.add_depended(&_nodes[qMakePair(
        (*it2)->host_id,
        (*it2)->service_id)]);
  }

  return ;
}

/**
 *  Start event correlation.
 */
void correlator::starting() {
  // Send engine state.
  logging::info(logging::medium) << "correlation: engine starting";
  misc::shared_ptr<engine_state> es(new engine_state);
  es->instance_id = _instance_id;
  es->started = true;
  _events.push_front(es);
  return ;
}

/**
 *  Stop event correlation.
 */
void correlator::stopping() {
  logging::info(logging::medium)
    << "correlation: engine shutting down";

  // Dump correlation state.
  _write_issues();

  // Close issues and issues parenting.
  /*time_t now(time(NULL));
  for (QMap<QPair<unsigned int, unsigned int>, node>::iterator
         it(_nodes.begin()),
         end(_nodes.end());
       it != end;
       ++it)
    if (it->my_issue.get()) {
      // Remove issue parenting.
      for (QList<node*>::const_iterator
             it2(it->depends_on().begin()),
             end2(it->depends_on().end());
           it2 != end2;
           ++it2)
        if ((*it2)->my_issue.get()) {
          misc::shared_ptr<issue_parent> p(new issue_parent);
          p->child_host_id = it->host_id;
          p->child_service_id = it->service_id;
          p->child_start_time = it->my_issue->start_time;
          p->parent_host_id = (*it2)->host_id;
          p->parent_service_id = (*it2)->service_id;
          p->parent_start_time = (*it2)->my_issue->start_time;
          p->start_time = (p->child_start_time > p->parent_start_time
                           ? p->child_start_time
                           : p->parent_start_time);
          p->end_time = now;
          _events.push_back(p);
        }
      bool all_parents(true);
      for (QList<node*>::const_iterator
             it2(it->parents().begin()),
             end2(it->parents().end());
           it2 != end2;
           ++it2)
        all_parents = (all_parents && (*it2)->my_issue.get());
      if (all_parents) {
        for (QList<node*>::const_iterator
               it2(it->parents().begin()),
               end2(it->parents().end());
             it2 != end2;
             ++it2) {
          misc::shared_ptr<issue_parent> p(new issue_parent);
          p->child_host_id = it->host_id;
          p->child_service_id = it->service_id;
          p->child_start_time = it->my_issue->start_time;
          p->parent_host_id = (*it2)->host_id;
          p->parent_service_id = (*it2)->service_id;
          p->parent_start_time = (*it2)->my_issue->start_time;
          p->start_time = (p->child_start_time > p->parent_start_time
                           ? p->child_start_time
                           : p->parent_start_time);
          p->end_time = now;
          _events.push_back(p);
        }
      }

      // XXX : missing state closing

      // Close issue itself.
      misc::shared_ptr<issue> i(new issue(*it->my_issue));
      i->end_time = now;
      _events.push_back(i);
    }
  */

  // Send engine state.
  misc::shared_ptr<engine_state> es(new engine_state);
  es->instance_id = _instance_id;
  es->started = false;
  _events.push_back(es);

  return ;
}

/**
 *  Reload configuration file.
 */
void correlator::update() {
  // Reload configuration file.
  QMap<QPair<unsigned int, unsigned int>, node> nodes;
  {
    parser p;
    p.parse(_correlation_file, false, nodes);
  }

  // Diff correlation file with current setup.
  QMap<QPair<unsigned int, unsigned int>, node>::iterator
    old_it(_nodes.begin()), old_end(_nodes.end());
  for (QMap<QPair<unsigned int, unsigned int>, node>::iterator
         new_it(nodes.begin()), new_end(nodes.end());
       new_it != new_end;
       ++new_it) {
    if ((old_it == old_end) || (new_it.key() < old_it.key())) {
      logging::info(logging::low) << "correlation: adding new node ("
        << new_it->host_id << ", " << new_it->service_id << ")";
      _nodes[new_it.key()] = *new_it;
    }
    else if (new_it.key() > old_it.key())
      old_it = _remove_node(old_it);
    else
      ++old_it;
  }
  while (old_it != old_end)
    old_it = _remove_node(old_it);

  return ;
}

/**
 *  Treat a new event.
 *
 *  @param[inout] e Event to process.
 *
 *  @return Number of events acknowledged (1).
 */
unsigned int correlator::write(misc::shared_ptr<io::data> const& e) {
  try {
    // Process event.
    (this->*_process_event)(e);
  }
  catch (exceptions::msg const& e) {
    logging::error(logging::high) << e.what() << " (ignored)";
  }
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_acknowledgement(
                   misc::shared_ptr<io::data> e) {
  // Cast event.
  neb::acknowledgement& ack(
    *static_cast<neb::acknowledgement*>(&*e));

  // Find associated node.
  logging::debug(logging::low)
    << "correlation: processing acknowledgement";
  QMap<QPair<unsigned int, unsigned int>, node>::iterator it;
  it = _nodes.find(qMakePair(ack.host_id, ack.service_id));
  if ((it != _nodes.end())
      && it->my_issue.get()
      && !it->my_issue->ack_time) {
    // Set issue acknowledgement time.
    logging::debug(logging::medium)
      << "correlation: setting issue of node (" << it->host_id << ", "
      << it->service_id << ") first acknowledgement time to "
      << static_cast<unsigned long long>(ack.entry_time);
    it->my_issue->ack_time = ack.entry_time;

    // Updated state.
    {
      misc::shared_ptr<state> state_update(
        ack.service_id ? static_cast<state*>(new service_state)
                       : static_cast<state*>(new host_state));
      state_update->instance_id = ack.instance_id;
      state_update->ack_time = ack.entry_time;
      state_update->current_state = it->state;
      state_update->host_id = it->host_id;
      state_update->in_downtime = it->in_downtime;
      state_update->service_id = it->service_id;
      state_update->start_time = it->since;
      _events.push_back(state_update);
    }

    // Send updated issue.
    _events.push_back(misc::shared_ptr<io::data>(
      new issue(*it->my_issue)));
  }
  return ;
}

/**
 *  Process a host_status or service_status event.
 *
 *  @param[in] e       Event to process.
 *  @param[in] is_host true if the event is a host_status.
 */
void correlator::_correlate_host_service_status(
                   misc::shared_ptr<io::data> e,
                   bool is_host) {
  neb::host_service_status& hss(
    *static_cast<neb::host_service_status*>(&*e));
  node* n;

  // Process only hard status.
  if (hss.state_type != 1) {
    logging::debug(logging::low)
      << "correlation: not processing non-hard host / service status";
    return ;
  }

  // Find node in appropriate list.
  if (is_host) {
    neb::host_status& hs(*static_cast<neb::host_status*>(&*e));
    QMap<QPair<unsigned int, unsigned int>, node>::iterator
      hs_it(_nodes.find(qMakePair(hs.host_id, 0u)));
    if (hs_it == _nodes.end())
      throw (exceptions::msg() << "correlation: invalid host status "
               "provided (" << hs.host_id << ")");
    n = &*hs_it;
  }
  else {
    neb::service_status& ss(*static_cast<neb::service_status*>(&*e));
    QMap<QPair<unsigned int, unsigned int>, node>::iterator
      ss_it(_nodes.find(qMakePair(ss.host_id, ss.service_id)));
    if (ss_it == _nodes.end())
      throw (exceptions::msg() << "correlation: invalid service "
                  "status provided (" << ss.host_id << ", "
               << ss.service_id << ")");
    n = &*ss_it;
  }

  if (hss.current_state
      && (hss.current_state != unknown_state(*n))
      && should_be_unknown(*n)) {
    logging::debug(logging::medium) << "correlation: retagging node ("
      << n->host_id << ", " << n->service_id << ") to unknown";
    hss.current_state = unknown_state(*n);
  }

  time_t now(hss.last_check);
  if (!now)
    now = hss.last_update;
  unsigned short old_state(n->state);
  bool state_changed(n->state != hss.current_state);
  if (state_changed
      || (n->in_downtime && !hss.scheduled_downtime_depth)
      || (!n->in_downtime && hss.scheduled_downtime_depth)) {

    // Update states.
    logging::debug(logging::medium) << "correlation: node ("
      << n->host_id << ", " << n->service_id << ") has new state event";
    // Now is the last update time if getting in/out of downtime.
    if ((n->in_downtime && !hss.scheduled_downtime_depth)
        || (!n->in_downtime && hss.scheduled_downtime_depth))
      now = hss.last_update;

    {
      // Old state.
      {
        misc::shared_ptr<state> state_update(
          is_host ? static_cast<state*>(new host_state)
                  : static_cast<state*>(new service_state));
        state_update->instance_id = hss.instance_id;
        state_update->current_state = n->state;
        state_update->end_time = now;
        state_update->host_id = n->host_id;
        state_update->in_downtime = n->in_downtime;
        state_update->service_id = n->service_id;
        state_update->start_time = n->since;
        if (!n->my_issue.get() || !n->my_issue->ack_time)
          state_update->ack_time = timestamp(-1);
        else if (n->my_issue->ack_time > n->since)
          state_update->ack_time = n->my_issue->ack_time;
        else
          state_update->ack_time = n->since;
        _events.push_back(state_update);
      }

      // Update node.
      n->in_downtime = hss.scheduled_downtime_depth;
      n->since = now;
      if (state_changed)
        n->state = hss.current_state;

      // New state.
      {
        misc::shared_ptr<state> state_update(
          is_host ? static_cast<state*>(new host_state)
                  : static_cast<state*>(new service_state));
        state_update->instance_id = hss.instance_id;
        state_update->ack_time = (hss.problem_has_been_acknowledged
                                  ? n->since
                                  : timestamp(-1));
        state_update->current_state = n->state;
        state_update->host_id = n->host_id;
        state_update->in_downtime = n->in_downtime;
        state_update->service_id = n->service_id;
        state_update->start_time = n->since;
        _events.push_back(state_update);
      }
    }
  }

  if (state_changed) {
    logging::info(logging::medium) << "correlation: node ("
      << n->host_id << ", " << n->service_id << ") changed status from "
      << old_state << " to " << hss.current_state;
    if (n->my_issue.get()) {
      // Issue is over.
      if (!n->state) {
        // Debug message.
        logging::info(logging::medium)
          << "correlation: issue on node (" << n->host_id
          << ", " << n->service_id << ") is over";

        // Issue parenting deletion.
        for (QList<node*>::const_iterator
               it(n->depends_on().begin()),
               end(n->depends_on().end());
             it != end;
             ++it)
          if ((*it)->my_issue.get()) {
            logging::debug(logging::low) << "correlation: deleting "
                 "issue parenting between dependent node ("
              << n->host_id << ", " << n->service_id << ") and node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            misc::shared_ptr<issue_parent> p(new issue_parent);
            p->child_host_id = n->host_id;
            p->child_instance_id = n->instance_id;
            p->child_service_id = n->service_id;
            p->child_start_time = n->my_issue->start_time;
            p->parent_host_id = (*it)->host_id;
            p->parent_instance_id = (*it)->instance_id;
            p->parent_service_id = (*it)->service_id;
            p->parent_start_time = (*it)->my_issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p);
          }
        for (QList<node*>::const_iterator
               it(n->depended_by().begin()),
               end(n->depended_by().end());
             it != end;
             ++it)
          if ((*it)->my_issue.get()) {
            logging::debug(logging::low) << "correlation: deleting "
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and dependent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            misc::shared_ptr<issue_parent> p(new issue_parent);
            p->child_host_id = (*it)->host_id;
            p->child_instance_id = (*it)->instance_id;
            p->child_service_id = (*it)->service_id;
            p->child_start_time = (*it)->my_issue->start_time;
            p->parent_host_id = n->host_id;
            p->parent_instance_id = n->instance_id;
            p->parent_service_id = n->service_id;
            p->parent_start_time = n->my_issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p);
          }
        bool all_parents(true);
        timestamp t(n->my_issue->start_time);
        for (QList<node*>::const_iterator
               it(n->parents().begin()),
               end(n->parents().end());
             all_parents && (it != end);
             ++it) {
          if (!(*it)->my_issue.get())
            all_parents = false;
          else if ((*it)->my_issue->start_time > t)
            t = (*it)->my_issue->start_time;
        }
        if (all_parents)
          for (QList<node*>::const_iterator
                 it(n->parents().begin()),
                 end(n->parents().end());
               it != end;
               ++it) {
            logging::debug(logging::low) << "correlation: deleting "
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and parent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            misc::shared_ptr<issue_parent> p(new issue_parent);
            p->child_host_id = n->host_id;
            p->child_instance_id = n->instance_id;
            p->child_service_id = n->service_id;
            p->child_start_time = n->my_issue->start_time;
            p->parent_host_id = (*it)->host_id;
            p->parent_instance_id = (*it)->instance_id;
            p->parent_service_id = (*it)->service_id;
            p->parent_start_time = (*it)->my_issue->start_time;
            p->start_time = t;
            p->end_time = now;
            _events.push_back(p);
          }
        for (QList<node*>::const_iterator
               it(n->children().begin()),
               end(n->children().end());
             it != end;
             ++it) {
          if ((*it)->my_issue.get()) {
            // Check that all parents of the node have an issue.
            all_parents = true;
            t = (*it)->my_issue->start_time;
            for (QList<node*>::const_iterator
                   it2((*it)->parents().begin()),
                   end2((*it)->parents().end());
                 all_parents && (it2 != end2);
                 ++it2) {
              if (!(*it2)->my_issue.get())
                all_parents = false;
              else if ((*it2)->my_issue->start_time > t)
                t = (*it2)->my_issue->start_time;
            }
            if (all_parents)
              for (QList<node*>::const_iterator
                     it2((*it)->parents().begin()),
                     end2((*it)->parents().end());
                   it2 != end2;
                   ++it2) {
                logging::debug(logging::low) << "correlation: "
                     "deleting issue parenting between node ("
                  << (*it)->host_id << ", " << (*it)->service_id
                  << ") and parent node (" << (*it2)->host_id << ", "
                  << (*it2)->service_id << ")";
                misc::shared_ptr<issue_parent> p(new issue_parent);
                p->child_host_id = (*it)->host_id;
                p->child_instance_id = (*it)->instance_id;
                p->child_service_id = (*it)->service_id;
                p->child_start_time = (*it)->my_issue->start_time;
                p->parent_host_id = (*it2)->host_id;
                p->parent_instance_id = (*it2)->instance_id;
                p->parent_service_id = (*it2)->service_id;
                p->parent_start_time = (*it2)->my_issue->start_time;
                p->start_time = t;
                p->end_time = now;
                _events.push_back(p);
              }
          }
        }

        // Terminate issue.
        n->my_issue->end_time = now;
        _events.push_back(
                  misc::shared_ptr<io::data>(n->my_issue.get()));
        n->my_issue.release();
      }
    }
    else {
      // Set issue.
      n->my_issue.reset(new issue);
      n->my_issue->instance_id = hss.instance_id;
      n->my_issue->host_id = n->host_id;
      n->my_issue->service_id = n->service_id;
      n->my_issue->start_time = now;

      // Store issue.
      _events.push_back(misc::shared_ptr<io::data>(
        new issue(*(n->my_issue))));

      // Declare parenting.
      _issue_parenting(n, true);
    }
  }
  else
    logging::debug(logging::low)
      << "correlation: nothing changed since last time on node ("
      << n->host_id << ", " << n->service_id << ")";
  return ;
}

/**
 *  Process a host_status event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_host_status(misc::shared_ptr<io::data> e) {
  logging::debug(logging::medium)
    << "correlation: processing host status";
  _correlate_host_service_status(e, true);
  return ;
}

/**
 *  Process a log_entry event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_log(misc::shared_ptr<io::data> e) {
  neb::log_entry* l(static_cast<neb::log_entry*>(&*e));
  node* n;

  QMap<QPair<unsigned int, unsigned int>, node>::iterator it(
    _nodes.find(qMakePair(l->host_id, l->service_id)));
  if (it != _nodes.end())
    n = &*it;
  else
    n = NULL;
  if (n && n->state) {
    issue* isu(_find_related_issue(*n));
    if (isu)
      l->issue_start_time = isu->start_time;
  }
  return ;
}

/**
 *  Process a service_status event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_service_status(
                   misc::shared_ptr<io::data> e) {
  logging::debug(logging::medium)
    << "correlation: processing service status";
  _correlate_host_service_status(e, false);
  return ;
}

/**
 *  Browse the parenting tree of the given node and find its ancestor's
 *  issue which causes it to be in undetermined state.
 *
 *  @param[in] n Base node.
 *
 *  @return The issue associated with node.
 */
issue* correlator::_find_related_issue(node& n) {
  issue* isu;
  if (n.state && n.my_issue.get())
    isu = n.my_issue.get();
  else {
    isu = NULL;
    for (QList<node*>::const_iterator
           it(n.depends_on().begin()),
           end(n.depends_on().end());
         it != end;
         ++it)
      if ((*it)->state) {
        isu = _find_related_issue(**it);
        break ;
      }
    if (!isu) {
      for (QList<node*>::const_iterator
             it(n.parents().begin()),
             end(n.parents().end());
           it != end;
           ++it)
        if ((*it)->state) {
          isu = _find_related_issue(**it);
          break ;
        }
    }
  }
  return (isu);
}

/**
 *  @brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  @param[in] c Object to copy.
 */
void correlator::_internal_copy(correlator const& c) {
  _instance_id = c._instance_id;
  _nodes = c._nodes;
  return ;
}

/**
 *  Send issue parenting.
 *
 *  @param[in] n    Node
 *  @param[in] full true to perform full linkage, false for half
 *                  linkage.
 */
void correlator::_issue_parenting(node* n, bool full) {
  if (full && (unknown_state(*n) == n->state)) {
    // Loop dependencies.
    for (QList<node*>::const_iterator
           it(n->depends_on().begin()),
           end(n->depends_on().end());
         it != end;
         ++it)
      if ((*it)->my_issue.get()) {
        logging::debug(logging::low) << "correlation: creating "
             "issue parenting between dependent node ("
          << n->host_id << ", " << n->service_id << ") and node ("
          << (*it)->host_id << ", " << (*it)->service_id << ")";
        misc::shared_ptr<issue_parent> parenting(new issue_parent);
        parenting->child_host_id = n->host_id;
        parenting->child_instance_id = n->instance_id;
        parenting->child_service_id = n->service_id;
        parenting->child_start_time = n->my_issue->start_time;
        parenting->parent_host_id = (*it)->host_id;
        parenting->parent_instance_id = (*it)->instance_id;
        parenting->parent_service_id = (*it)->service_id;
        parenting->parent_start_time = (*it)->my_issue->start_time;
        parenting->start_time = n->my_issue->start_time;
        _events.push_back(parenting);
      }

    // Loop parents.
    bool all_parent_issue(true);
    for (QList<node*>::const_iterator
           it(n->parents().begin()),
           end(n->parents().end());
         it != end;
         ++it)
      all_parent_issue = (all_parent_issue && (*it)->my_issue.get());
    if (all_parent_issue) {
      for (QList<node*>::const_iterator
             it(n->parents().begin()),
             end(n->parents().end());
           it != end;
           ++it) {
        logging::debug(logging::low) << "correlation: creating "
             "issue parenting between node (" << n->host_id << ", "
          << n->service_id << ") and parent node ("
          << (*it)->host_id << ", " << (*it)->service_id << ")";
        misc::shared_ptr<issue_parent> parenting(new issue_parent);
        parenting->child_host_id = n->host_id;
        parenting->child_instance_id = n->instance_id;
        parenting->child_service_id = n->service_id;
        parenting->child_start_time = n->my_issue->start_time;
        parenting->parent_host_id = (*it)->host_id;
        parenting->parent_instance_id = (*it)->instance_id;
        parenting->parent_service_id = (*it)->service_id;
        parenting->parent_start_time = (*it)->my_issue->start_time;
        parenting->start_time = n->my_issue->start_time;
        _events.push_back(parenting);
      }
    }
  }

  // Loop dependant nodes.
  for (QList<node*>::const_iterator
         it(n->depended_by().begin()),
         end(n->depended_by().end());
       it != end;
       ++it)
    if ((*it)->my_issue.get()) {
      logging::debug(logging::low) << "correlation: creating "
           "issue parenting between node (" << n->host_id << ", "
        << n->service_id << ") and dependent node ("
        << (*it)->host_id << ", " << (*it)->service_id << ")";
      misc::shared_ptr<issue_parent> parenting(new issue_parent);
      parenting->child_host_id = (*it)->host_id;
      parenting->child_instance_id = (*it)->instance_id;
      parenting->child_service_id = (*it)->service_id;
      parenting->child_start_time = (*it)->my_issue->start_time;
      parenting->parent_host_id = n->host_id;
      parenting->parent_instance_id = n->instance_id;
      parenting->parent_service_id = n->service_id;
      parenting->parent_start_time = n->my_issue->start_time;
      parenting->start_time = n->my_issue->start_time;
      _events.push_back(parenting);
    }

  // Loop children.
  for (QList<node*>::const_iterator
         it(n->children().begin()),
         end(n->children().end());
       it != end;
       ++it)
    if ((*it)->my_issue.get()) {
      // Check that all parents of the node have an issue.
      bool all_parent_issue(true);
      for (QList<node*>::const_iterator
             it2((*it)->parents().begin()),
             end2((*it)->parents().end());
           it2 != end2;
           ++it2)
        all_parent_issue = (all_parent_issue && (*it2)->my_issue.get());
      if (all_parent_issue)
        for (QList<node*>::const_iterator
               it2((*it)->parents().begin()),
               end2((*it)->parents().end());
             it2 != end2;
             ++it2) {
          logging::debug(logging::low) << "correlation: "
               "creating issue parenting between node ("
            << (*it)->host_id << ", " << (*it)->service_id
            << ") and parent node (" << (*it2)->host_id << ", "
            << (*it2)->service_id << ")";
          misc::shared_ptr<issue_parent> parenting(new issue_parent);
          parenting->child_host_id = (*it)->host_id;
          parenting->child_instance_id = (*it)->instance_id;
          parenting->child_service_id = (*it)->service_id;
          parenting->child_start_time = (*it)->my_issue->start_time;
          parenting->parent_host_id = (*it2)->host_id;
          parenting->parent_instance_id = (*it2)->instance_id;
          parenting->parent_service_id = (*it2)->service_id;
          parenting->parent_start_time = (*it2)->my_issue->start_time;
          parenting->start_time = n->my_issue->start_time;
          _events.push_back(parenting);
        }
    }
  return ;
}

/**
 *  Remove a node.
 */
QMap<QPair<unsigned int, unsigned int>, node>::iterator correlator::_remove_node(QMap<QPair<unsigned int, unsigned int>, node>::iterator it) {
  // Close issue and issue parenting.
  logging::debug(logging::medium)
    << "correlation: submitting OK status for node ("
    << it->host_id << ", " << it->service_id << ") before deletion";
  if (it->service_id) {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->instance_id = it->instance_id;
    ss->current_state = 0;
    ss->state_type = 1;
    ss->host_id = it->host_id;
    ss->service_id = it->service_id;
    _correlate_service_status(ss);
  }
  else {
    misc::shared_ptr<neb::host_status> hs(new neb::host_status);
    hs->instance_id = it->instance_id;
    hs->current_state = 0;
    hs->state_type = 1;
    hs->host_id = it->host_id;
    _correlate_host_status(hs);
  }

  // Delete node for real.
  return (_nodes.erase(it));
}

/**
 *  Process event on active mode.
 *
 *  @param[inout] e Event to process.
 */
void correlator::_process_event_on_active(
                   misc::shared_ptr<io::data> const& e) {
  if (e.isNull())
    return ;
  unsigned int e_type(e->type());
  if ((e_type == neb::service_status::static_type())
        || (e_type == neb::service::static_type()))
    _correlate_service_status(e);
  else if ((e_type == neb::host_status::static_type())
           || (e_type == neb::host::static_type()))
    _correlate_host_status(e);
  else if (e_type == neb::log_entry::static_type())
    _correlate_log(e);
  else if (e_type == neb::acknowledgement::static_type())
    _correlate_acknowledgement(e);
  else if (e_type == neb::instance_status::static_type()) {
    // Dump retention file.
    static time_t next_dump(0);
    time_t now(time(NULL));
    if (now > next_dump) {
      _write_issues();
      next_dump = now + 60;
    }
  }
}

/**
 *  Process event on passive mode.
 *
 *  @param[inout] e Event to process.
 */
void correlator::_process_event_on_passive(
                   misc::shared_ptr<io::data> const& e) {
  unsigned int e_type(e->type());

  logging::debug(logging::low)
    << "correlation:: process passive event (" << e_type << ")";

  // if (e_type == io::events::data_type<io::events::correlation, correlation::de_engine_state>::value)
  //   ;
  if (e_type == correlation::host_state::static_type())
    _update_host_service_state(e.staticCast<state>());
  else if (e_type == correlation::issue::static_type())
    _update_issue(e.staticCast<issue>());
  // else if (e_type == io::events::data_type<io::events::correlation, correlation::de_issue_parent>::value)
  //   _update_issue_parent(e.staticCast<issue_parent>());
  else if (e_type == correlation::service_state::static_type())
    _update_host_service_state(e.staticCast<state>());

  return ;
}

/**
 *  Update internal host or service state.
 *
 *  @param[inout] s The host or service state.
 */
void correlator::_update_host_service_state(misc::shared_ptr<state> s) {
  logging::debug(logging::low)
    << "correlation:: process passive state event: node("
    << s->host_id << ", " << s->service_id << ")";

  QMap<QPair<unsigned int, unsigned int>, node>::iterator
    it(_nodes.find(qMakePair(s->host_id, s->service_id)));
  if (it != _nodes.end()) {
    node& n(it.value());
    n.since = s->start_time;
    n.in_downtime = s->in_downtime;
    n.state = s->current_state;
  }

  return ;
}

/**
 *  Update internal issue.
 *
 *  @param[inout] i The issue.
 */
void correlator::_update_issue(misc::shared_ptr<issue> i) {
  logging::debug(logging::low)
    << "correlation:: process passive issue event: node("
    << i->host_id << ", " << i->service_id << ")";

  QMap<QPair<unsigned int, unsigned int>, node>::iterator
    it(_nodes.find(qMakePair(i->host_id, i->service_id)));
  if (it != _nodes.end()) {
    node& n(it.value());
    if (i->end_time)
      n.my_issue.reset();
    else {
      if (!n.my_issue.get()) {
        n.my_issue.reset(new issue);
        n.my_issue->instance_id = i->instance_id;
        n.my_issue->host_id = i->host_id;
        n.my_issue->service_id = i->service_id;
        n.my_issue->start_time = i->start_time;
      }
      n.my_issue->ack_time = i->ack_time;
    }
  }

  return ;
}

/**
 *  Dump issues and issues parenting to a file.
 */
void correlator::_write_issues() {
  if (!_retention_file.isEmpty()) {
    // Write issue file
    persistent_cache f(_retention_file.toStdString());
    f.transaction();
    for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
           it(_nodes.begin()),
           end(_nodes.end());
         it != end;
         ++it) {
      if (it->my_issue.get())
      f.add(misc::shared_ptr<issue>(new issue(*it->my_issue)));
    }
    f.commit();

    logging::config(logging::medium) << "correlation: finished "
      "writing retention file: " << _retention_file;
  }

  return ;
}
