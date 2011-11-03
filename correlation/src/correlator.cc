/*
** Copyright 2009-2011 Merethis
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

#include <assert.h>
#include <QDomDocument>
#include <QDomElement>
#include <time.h>
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"

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
    for (QList<node*>::const_iterator it = n.parents().begin(),
           end = n.parents().end();
         it != end;
         ++it)
      all_parents_down = (all_parents_down && (*it)->state);
  }
  else
    all_parents_down = false;

  // Check dependencies.
  one_dependency_down = false;
  for (QList<node*>::const_iterator it = n.depends_on().begin(),
         end = n.depends_on().end();
       it != end;
       ++it)
    one_dependency_down = (one_dependency_down || (*it)->state);

  // Debug message.
  if (all_parents_down && one_dependency_down)
    logging::debug << logging::MEDIUM
      << "correlation: unknown state of node (" << n.host_id << ", "
      << n.service_id << ") is triggered by parenting AND dependencies";
  else if (all_parents_down)
    logging::debug << logging::MEDIUM
      << "correlation: unknown state of node (" << n.host_id << ", "
      << n.service_id << ") is triggered by parenting";
  else if (one_dependency_down)
    logging::debug << logging::MEDIUM
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
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_acknowledgement(QSharedPointer<io::data> e) {
  // Cast event.
  neb::acknowledgement& ack(
    *static_cast<neb::acknowledgement*>(&*e));

  // Find associated node.
  logging::debug(logging::low)
    << "correlation: processing acknowledgement";
  QMap<QPair<unsigned int, unsigned int>, node>::iterator it;
  it = _nodes.find(qMakePair(ack.host_id, ack.service_id));
  if ((it != _nodes.end())
      && !it->my_issue.isNull()
      && !it->my_issue->ack_time) {
    // Set issue acknowledgement time.
    logging::debug(logging::medium)
      << "correlation: setting issue of node (" << it->host_id << ", "
      << it->service_id << ") first acknowledgement time to "
      << static_cast<unsigned long long>(ack.entry_time);
    it->my_issue->ack_time = ack.entry_time;

    time_t now(time(NULL));

    // Old state.
    {
      QSharedPointer<state> state_update(
        ack.service_id ? static_cast<state*>(new service_state)
                       : static_cast<state*>(new host_state));
      state_update->current_state = it->state;
      state_update->end_time = now;
      state_update->host_id = it->host_id;
      state_update->in_downtime = it->in_downtime;
      state_update->service_id = it->service_id;
      state_update->start_time = it->since;
      _events.push_back(state_update.staticCast<io::data>());
    }

    // New state.
    {
      QSharedPointer<state> state_update(
        ack.service_id ? static_cast<state*>(new service_state)
                       : static_cast<state*>(new host_state));
      state_update->ack_time = ack.entry_time;
      state_update->current_state = it->state;
      state_update->host_id = it->host_id;
      state_update->in_downtime = it->in_downtime;
      state_update->service_id = it->service_id;
      state_update->start_time = now;
      _events.push_back(state_update.staticCast<io::data>());
    }

    // Send updated issue.
    _events.push_back(QSharedPointer<io::data>(
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
void correlator::_correlate_host_service_status(QSharedPointer<io::data> e,
                                                bool is_host) {
  neb::host_service_status& hss(
    *static_cast<neb::host_service_status*>(&*e));
  node* n;

  // Process only hard status.
  if (hss.state_type != 1) {
    logging::debug << logging::LOW
      << "correlation: not processing non-hard host / service status";
    return ;
  }

  // Find node in appropriate list.
  if (is_host) {
    neb::host_status& hs(*static_cast<neb::host_status*>(&*e));
    QMap<QPair<unsigned int, unsigned int>, node>::iterator
      hs_it(_nodes.find(qMakePair(hs.host_id, 0u)));
    if (hs_it == _nodes.end())
      throw (exceptions::msg() << "correlation: invalid host status " \
               "provided (" << hs.host_id);
    n = &*hs_it;
  }
  else {
    neb::service_status& ss(*static_cast<neb::service_status*>(&*e));
    QMap<QPair<unsigned int, unsigned int>, node>::iterator
      ss_it(_nodes.find(qMakePair(ss.host_id, ss.service_id)));
    if (ss_it == _nodes.end())
      throw (exceptions::msg() << "correlation: invalid service " \
                  "status provided (" << ss.host_id << ", "
               << ss.service_id << ")");
    n = &*ss_it;
  }

  if (hss.current_state
      && (hss.current_state != unknown_state(*n))
      && should_be_unknown(*n)) {
    logging::debug << logging::MEDIUM
      << "correlation: retagging node (" << n->host_id << ", "
      << n->service_id << ") to unknown";
    hss.current_state = unknown_state(*n);
  }

  time_t now(time(NULL));
  bool state_changed(n->state != hss.current_state);
  if (state_changed
      || (n->in_downtime && !hss.scheduled_downtime_depth)
      || (!n->in_downtime && hss.scheduled_downtime_depth)) {

    // Update states.
    logging::debug << logging::MEDIUM
      << "correlation: node (" << n->host_id << ", "
      << n->service_id << ") has new state event";
    {
      // Old state.
      {
        QSharedPointer<state> state_update(
          is_host ? static_cast<state*>(new host_state)
                  : static_cast<state*>(new service_state));
        state_update->ack_time =
          (n->my_issue.isNull() ? 0 : n->my_issue->ack_time);
        state_update->current_state = n->state;
        state_update->end_time = now;
        state_update->host_id = n->host_id;
        state_update->in_downtime = n->in_downtime;
        state_update->service_id = n->service_id;
        if (n->my_issue.isNull())
          state_update->start_time = n->since;
        else
          state_update->start_time =
            ((n->since < n->my_issue->ack_time)
              ? n->my_issue->ack_time
              : n->since);
        _events.push_back(state_update.staticCast<io::data>());
      }

      // Update node.
      n->in_downtime = hss.scheduled_downtime_depth;
      if (state_changed) {
        n->since = now;
        n->state = hss.current_state;
      }

      // New state.
      {
        QSharedPointer<state> state_update(
          is_host ? static_cast<state*>(new host_state)
                  : static_cast<state*>(new service_state));
        state_update->ack_time =
          ((n->my_issue.isNull() || !n->state)
            ? 0
            : n->my_issue->ack_time);
        state_update->current_state = n->state;
        state_update->host_id = n->host_id;
        state_update->in_downtime = n->in_downtime;
        state_update->service_id = n->service_id;
        state_update->start_time = n->since;
        _events.push_back(state_update.staticCast<io::data>());
      }
    }
  }

  if (state_changed) {
    logging::debug << logging::MEDIUM
      << "correlation: node (" << n->host_id << ", " << n->service_id
      << ") changed status from " << n->state
      << " to " << hss.current_state;
    if (n->my_issue) {
      // Issue is over.
      if (!n->state) {
        // Debug message.
        logging::debug << logging::MEDIUM
          << "correlation: issue on node (" << n->host_id
          << ", " << n->service_id << ") is over";
        
        // Issue parenting deletion.
        for (QList<node*>::const_iterator
               it = n->depends_on().begin(),
               end = n->depends_on().end();
             it != end;
             ++it)
          if ((*it)->my_issue) {
            logging::debug << logging::LOW << "correlation: deleting " \
                 "issue parenting between dependent node ("
              << n->host_id << ", " << n->service_id << ") and node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            QSharedPointer<issue_parent> p(new issue_parent);
            p->child_host_id = n->host_id;
            p->child_service_id = n->service_id;
            p->child_start_time = n->my_issue->start_time;
            p->parent_host_id = (*it)->host_id;
            p->parent_service_id = (*it)->service_id;
            p->parent_start_time = (*it)->my_issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.staticCast<io::data>());
          }
        for (QList<node*>::const_iterator
               it = n->depended_by().begin(),
               end = n->depended_by().end();
             it != end;
             ++it)
          if ((*it)->my_issue) {
            logging::debug << logging::LOW << "correlation: deleting " \
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and dependent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            QSharedPointer<issue_parent> p(new issue_parent);
            p->child_host_id = (*it)->host_id;
            p->child_service_id = (*it)->service_id;
            p->child_start_time = (*it)->my_issue->start_time;
            p->parent_host_id = n->host_id;
            p->parent_service_id = n->service_id;
            p->parent_start_time = n->my_issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.staticCast<io::data>());
          }
        bool all_parents = true;
        for (QList<node*>::const_iterator
               it = n->parents().begin(),
               end = n->parents().end();
             it != end;
             ++it)
          if (!(*it)->my_issue)
            all_parents = false;
        if (all_parents)
          for (QList<node*>::const_iterator
                 it = n->parents().begin(),
                 end = n->parents().end();
               it != end;
               ++it) {
            logging::debug << logging::LOW << "correlation: deleting " \
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and parent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            QSharedPointer<issue_parent> p(new issue_parent);
            p->child_host_id = n->host_id;
            p->child_service_id = n->service_id;
            p->child_start_time = n->my_issue->start_time;
            p->parent_host_id = (*it)->host_id;
            p->parent_service_id = (*it)->service_id;
            p->parent_start_time = (*it)->my_issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.staticCast<io::data>());
          }
        for (QList<node*>::const_iterator
               it = n->children().begin(),
               end = n->children().end();
             it != end;
             ++it) {
          if (!(*it)->my_issue.isNull()) {
            // Check that all parents of the node have an issue.
            all_parents = true;
            for (QList<node*>::const_iterator
                   it2 = (*it)->parents().begin(),
                   end2 = (*it)->parents().end();
                 it2 != end2;
                 ++it2)
              all_parents = (all_parents && !(*it2)->my_issue.isNull());
            if (all_parents)
              for (QList<node*>::const_iterator
                     it2 = (*it)->parents().begin(),
                     end2 = (*it)->parents().end();
                   it2 != end2;
                   ++it2) {
                logging::debug << logging::LOW << "correlation: " \
                     "deleting issue parenting between node ("
                  << (*it)->host_id << ", " << (*it)->service_id
                  << ") and parent node (" << (*it2)->host_id << ", "
                  << (*it2)->service_id << ")";
                QSharedPointer<issue_parent> p(new issue_parent);
                p->child_host_id = (*it)->host_id;
                p->child_service_id = (*it)->service_id;
                p->child_start_time = (*it)->my_issue->start_time;
                p->parent_host_id = (*it2)->host_id;
                p->parent_service_id = (*it2)->service_id;
                p->parent_start_time = (*it2)->my_issue->start_time;
                p->start_time = (p->child_start_time > p->parent_start_time
                                 ? p->child_start_time
                                 : p->parent_start_time);
                p->end_time = now;
                _events.push_back(p.staticCast<io::data>());
              }
          }
        }

        // Terminate issue.
        n->my_issue->end_time = now;
        _events.push_back(QSharedPointer<io::data>(n->my_issue.data()));
        n->my_issue.take();
      }
    }
    else {
      // Set issue.
      n->my_issue.reset(new issue);
      n->my_issue->host_id = n->host_id;
      n->my_issue->service_id = n->service_id;
      n->my_issue->start_time = now;

      // Store issue.
      _events.push_back(QSharedPointer<io::data>(
        new issue(*(n->my_issue))));

      // Declare parenting.
      _issue_parenting(n, true);
    }
  }
  else
    logging::debug << logging::LOW
      << "correlation: nothing changed since last time on node ("
      << n->host_id << ", " << n->service_id << ")";
  return ;
}

/**
 *  Process a host_status event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_host_status(QSharedPointer<io::data> e) {
  logging::debug << logging::MEDIUM
    << "correlation: processing host status";
  _correlate_host_service_status(e, true);
  return ;
}

/**
 *  Process a log_entry event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_log(QSharedPointer<io::data> e) {
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
void correlator::_correlate_service_status(QSharedPointer<io::data> e) {
  logging::debug << logging::MEDIUM
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
  if (n.state && !n.my_issue.isNull())
    isu = n.my_issue.data();
  else {
    isu = NULL;
    for (QList<node*>::const_iterator
           it = n.depends_on().begin(),
           end = n.depends_on().end();
         it != end;
         ++it)
      if ((*it)->state) {
        isu = _find_related_issue(**it);
        break ;
      }
    if (!isu) {
      for (QList<node*>::const_iterator
             it = n.parents().begin(),
             end = n.parents().end();
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
           it = n->depends_on().begin(),
           end = n->depends_on().end();
         it != end;
         ++it)
      if ((*it)->my_issue) {
        logging::debug << logging::LOW << "correlation: creating " \
             "issue parenting between dependent node ("
          << n->host_id << ", " << n->service_id << ") and node ("
          << (*it)->host_id << ", " << (*it)->service_id << ")";
        QSharedPointer<issue_parent> parenting(new issue_parent);
        parenting->child_host_id = n->host_id;
        parenting->child_service_id = n->service_id;
        parenting->child_start_time = n->my_issue->start_time;
        parenting->parent_host_id = (*it)->host_id;
        parenting->parent_service_id = (*it)->service_id;
        parenting->parent_start_time = (*it)->my_issue->start_time;
        parenting->start_time = n->my_issue->start_time;
        _events.push_back(parenting.staticCast<io::data>());
      }

    // Loop parents.
    bool all_parent_issue = true;
    for (QList<node*>::const_iterator
           it = n->parents().begin(),
           end = n->parents().end();
         it != end;
         ++it)
      all_parent_issue = (all_parent_issue && (*it)->my_issue);
    if (all_parent_issue) {
      for (QList<node*>::const_iterator
             it = n->parents().begin(),
             end = n->parents().end();
           it != end;
           ++it) {
        logging::debug << logging::LOW << "correlation: creating "      \
             "issue parenting between node (" << n->host_id << ", "
          << n->service_id << ") and parent node ("
          << (*it)->host_id << ", " << (*it)->service_id << ")";
        QSharedPointer<issue_parent> parenting(new issue_parent);
        parenting->child_host_id = n->host_id;
        parenting->child_service_id = n->service_id;
        parenting->child_start_time = n->my_issue->start_time;
        parenting->parent_host_id = (*it)->host_id;
        parenting->parent_service_id = (*it)->service_id;
        parenting->parent_start_time = (*it)->my_issue->start_time;
        parenting->start_time = n->my_issue->start_time;
        _events.push_back(parenting.staticCast<io::data>());
      }
    }
  }

  // Loop dependant nodes.
  for (QList<node*>::const_iterator
         it = n->depended_by().begin(),
         end = n->depended_by().end();
       it != end;
       ++it)
    if (!(*it)->my_issue.isNull()) {
      logging::debug << logging::LOW << "correlation: creating " \
           "issue parenting between node (" << n->host_id << ", "
        << n->service_id << ") and dependent node ("
        << (*it)->host_id << ", " << (*it)->service_id << ")";
      QSharedPointer<issue_parent> parenting(new issue_parent);
      parenting->child_host_id = (*it)->host_id;
      parenting->child_service_id = (*it)->service_id;
      parenting->child_start_time = (*it)->my_issue->start_time;
      parenting->parent_host_id = n->host_id;
      parenting->parent_service_id = n->service_id;
      parenting->parent_start_time = n->my_issue->start_time;
      parenting->start_time = n->my_issue->start_time;
      _events.push_back(parenting.staticCast<io::data>());
    }

  // Loop children.
  for (QList<node*>::const_iterator
         it = n->children().begin(),
         end = n->children().end();
       it != end;
       ++it)
    if (!(*it)->my_issue.isNull()) {
      // Check that all parents of the node have an issue.
      bool all_parent_issue = true;
      for (QList<node*>::const_iterator
             it2 = (*it)->parents().begin(),
             end2 = (*it)->parents().end();
           it2 != end2;
           ++it2)
        all_parent_issue = (all_parent_issue && !(*it2)->my_issue.isNull());
      if (all_parent_issue)
        for (QList<node*>::const_iterator
               it2 = (*it)->parents().begin(),
               end2 = (*it)->parents().end();
             it2 != end2;
             ++it2) {
          logging::debug << logging::LOW << "correlation: " \
               "creating issue parenting between node ("
            << (*it)->host_id << ", " << (*it)->service_id
            << ") and parent node (" << (*it2)->host_id << ", "
            << (*it2)->service_id << ")";
          QSharedPointer<issue_parent> parenting(new issue_parent);
          parenting->child_host_id = (*it)->host_id;
          parenting->child_service_id = (*it)->service_id;
          parenting->child_start_time = (*it)->my_issue->start_time;
          parenting->parent_host_id = (*it2)->host_id;
          parenting->parent_service_id = (*it2)->service_id;
          parenting->parent_start_time = (*it2)->my_issue->start_time;
          parenting->start_time = (*it2)->my_issue->start_time;
          _events.push_back(parenting.staticCast<io::data>());
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
    QSharedPointer<neb::service_status> ss(new neb::service_status);
    ss->current_state = 0;
    ss->state_type = 1;
    ss->host_id = it->host_id;
    ss->service_id = it->service_id;
    _correlate_service_status(ss.staticCast<io::data>());
  }
  else {
    QSharedPointer<neb::host_status> hs(new neb::host_status);
    hs->current_state = 0;
    hs->state_type = 1;
    hs->host_id = it->host_id;
    _correlate_host_status(hs.staticCast<io::data>());
  }

  // Delete node for real.
  return (_nodes.erase(it));
}

/**
 *  Dump issues and issues parenting to a file.
 */
void correlator::_write_issues() {
  if (!_retention_file.isEmpty()) {
    // Prepare XML document.
    QDomDocument doc;
    QDomElement root(doc.createElement("centreonbroker"));
    doc.appendChild(root);

    // Dump nodes and issues.
    for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
           it = _nodes.begin(),
           end = _nodes.end();
         it != end;
         ++it) {
      {
        QDomElement elem;
        if (it->service_id) {
          elem = doc.createElement("service");
          elem.setAttribute(
            "host",
            QString("%1").arg(it->host_id));
          elem.setAttribute(
            "id",
            QString("%1").arg(it->service_id));
        }
        else {
          elem = doc.createElement("host");
          elem.setAttribute(
            "id",
            QString("%1").arg(it->host_id));
        }
        elem.setAttribute(
          "since",
          QString("%1").arg(it->since));
        elem.setAttribute(
          "state",
          QString("%1").arg(it->state));
        root.appendChild(elem);
      }
      if (!it->my_issue.isNull()) {
        QDomElement elem(doc.createElement("issue"));
        elem.setAttribute(
          "ack_time",
          QString("%1").arg(it->my_issue->ack_time));
        elem.setAttribute(
          "host",
          QString("%1").arg(it->my_issue->host_id));
        elem.setAttribute(
          "service",
          QString("%1").arg(it->my_issue->service_id));
        elem.setAttribute(
          "start_time",
          QString("%1").arg(it->my_issue->start_time));
        root.appendChild(elem);
      }
    }

    // Write issue file
    QFile f(_retention_file);
    if (!f.open(QIODevice::WriteOnly))
      logging::config(logging::high) << "correlation: could not write" \
        " retention file: " << f.errorString();
    else {
      QByteArray ba(doc.toByteArray());
      qint64 wb;
      while (ba.size() > 0) {
        f.waitForBytesWritten(-1);
        wb = f.write(ba);
        if (wb <= 0) {
          logging::config(logging::medium) << "correlation: finished " \
            "writing retention file: " << f.errorString();
          break ;
        }
        ba.remove(0, wb);
      }
    }
  }

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
correlator::correlator() {}

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
void correlator::load(QString const& correlation_file,
                      QString const& retention_file) {
  // Set files.
  _correlation_file = correlation_file;
  _retention_file = retention_file;

  // Load configuration file.
  {
    logging::debug(logging::medium)
      << "correlation: loading configuration file";
    parser p;
    p.parse(_correlation_file, false, _nodes);
  }

  // Load retention file.
  if (!_retention_file.isEmpty() && QFile::exists(_retention_file)) {
    logging::debug(logging::medium)
      << "correlation: loading retention file";
    parser p;
    p.parse(_retention_file, true, _nodes);
  }

  // Reopen issues.
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it = _nodes.begin(),
         end = _nodes.end();
       it != end;
       ++it)
    if (!it->my_issue.isNull()) {
      logging::debug(logging::medium)
        << "correlation: reopening issue of node ("
        << it->host_id << ", " << it->service_id << ")";
      _events.push_back(
        QSharedPointer<io::data>(new issue(*(it->my_issue))));
    }

  // Re-link issues.
  for (QMap<QPair<unsigned int, unsigned int>, node>::iterator
         it = _nodes.begin(),
         end = _nodes.end();
       it != end;
       ++it)
    if (!it->my_issue.isNull())
      _issue_parenting(&*it, false);

  return ;
}

/**
 *  Get the next available correlated event.
 *
 *  @return The next available correlated event.
 */
QSharedPointer<io::data> correlator::read() {
  QSharedPointer<io::data> e;
  if (!_events.empty()) {
    e = _events.front();
    _events.pop_front();
  }
  return (e);
}

/**
 *  Set correlator state.
 *
 *  @param[in] state New correlation state.
 */
void correlator::set_state(QMap<QPair<unsigned int, unsigned int>, node> const& state) {
  // Copy nodes.
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it = state.begin(),
         end = state.end();
       it != end;
       ++it) {
    node& n(_nodes[qMakePair(it->host_id, it->service_id)]);
    n.host_id = it->host_id;
    n.service_id = it->service_id;
    n.since = it->since;
    n.state = it->state;
    if (!it->my_issue.isNull())
      n.my_issue.reset(new issue(*it->my_issue));
  }

  // Copy node links.
  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it = state.begin(),
         end = state.end();
       it != end;
       ++it) {
    node& n(_nodes[qMakePair(it->host_id, it->service_id)]);
    // Copy children.
    for (QList<node*>::const_iterator
           it2 = it->children().begin(),
           end2 = it->children().end();
         it2 != end2;
         ++it2)
      n.add_child(&_nodes[qMakePair(
        (*it2)->host_id,
        (*it2)->service_id)]);
    // Copy dependencies.
    for (QList<node*>::const_iterator
           it2 = it->depended_by().begin(),
           end2 = it->depended_by().end();
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
  logging::debug << logging::MEDIUM << "correlation: engine starting";
  QSharedPointer<engine_state> es(new engine_state);
  es->started = true;
  _events.push_front(es.staticCast<io::data>());
  return ;
}

/**
 *  Stop event correlation.
 */
void correlator::stopping() {
  logging::debug << logging::MEDIUM
    << "correlation: engine shutting down";

  // Dump correlation state.
  _write_issues();

  // Close issues and issues parenting.
  /*time_t now(time(NULL));
  for (QMap<QPair<unsigned int, unsigned int>, node>::iterator
         it = _nodes.begin(),
         end = _nodes.end();
       it != end;
       ++it)
    if (!it->my_issue.isNull()) {
      // Remove issue parenting.
      for (QList<node*>::const_iterator
             it2 = it->depends_on().begin(),
             end2 = it->depends_on().end();
           it2 != end2;
           ++it2)
        if (!(*it2)->my_issue.isNull()) {
          QSharedPointer<issue_parent> p(new issue_parent);
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
          _events.push_back(p.staticCast<io::data>());
        }
      bool all_parents(true);
      for (QList<node*>::const_iterator
             it2 = it->parents().begin(),
             end2 = it->parents().end();
           it2 != end2;
           ++it2)
        all_parents = (all_parents && !(*it2)->my_issue.isNull());
      if (all_parents) {
        for (QList<node*>::const_iterator
               it2 = it->parents().begin(),
               end2 = it->parents().end();
             it2 != end2;
             ++it2) {
          QSharedPointer<issue_parent> p(new issue_parent);
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
          _events.push_back(p.staticCast<io::data>());
        }
      }

      // XXX : missing state closing

      // Close issue itself.
      QSharedPointer<issue> i(new issue(*it->my_issue));
      i->end_time = now;
      _events.push_back(i.staticCast<io::data>());
    }
  */

  // Send engine state.
  QSharedPointer<engine_state> es(new engine_state);
  es->started = false;
  _events.push_back(es.staticCast<io::data>());

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
      logging::debug(logging::low) << "correlation: adding new node ("
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
 */
void correlator::write(QSharedPointer<io::data> e) {
  try {
    // Process event.
    if ("com::centreon::broker::neb::host" == e->type())
      _correlate_host_status(e);
    else if ("com::centreon::broker::neb::host_status" == e->type())
      _correlate_host_status(e);
    else if ("com::centreon::broker::neb::service" == e->type())
      _correlate_service_status(e);
    else if ("com::centreon::broker::neb::service_status" == e->type())
      _correlate_service_status(e);
    else if ("com::centreon::broker::neb::log_entry" == e->type())
      _correlate_log(e);
    else if ("com::centreon::broker::neb::acknowledgement" == e->type())
      _correlate_acknowledgement(e);

    // Dump retention file.
    static time_t next_dump(0);
    time_t now(time(NULL));
    if (now > next_dump) {
      _write_issues();
      next_dump = now + 60;
    }
  }
  catch (exceptions::msg const& e) {
    logging::error << logging::HIGH << e.what();
  }
  return ;
}
