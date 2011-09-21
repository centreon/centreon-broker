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

#include <assert.h>
#include <memory>
#include <time.h>
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/host.hh"
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
  if (n.parents.size()) {
    all_parents_down = true;
    for (QList<node*>::const_iterator it = n.parents.begin(),
           end = n.parents.end();
         it != end;
         ++it)
      all_parents_down = (all_parents_down && (*it)->state);
  }
  else
    all_parents_down = false;

  // Check dependencies.
  one_dependency_down = false;
  for (QList<node*>::const_iterator it = n.depends_on.begin(),
         end = n.depends_on.end();
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
    std::map<unsigned int, node>::iterator hs_it(_hosts.find(hs.host_id));
    if (hs_it == _hosts.end())
      throw (exceptions::msg() << "correlation: invalid host status " \
               "provided (" << hs.host_id);
    n = &hs_it->second;
  }
  else {
    neb::service_status& ss(*static_cast<neb::service_status*>(&*e));
    std::map<std::pair<unsigned int, unsigned int>, node>::iterator ss_it(
      _services.find(std::make_pair(ss.host_id, ss.service_id)));
    if (ss_it == _services.end())
      throw (exceptions::msg() << "correlation: invalid service " \
                  "status provided (" << ss.host_id << ", "
               << ss.service_id << ")");
    n = &ss_it->second;
  }

  if (hss.current_state
      && (hss.current_state != unknown_state(*n))
      && should_be_unknown(*n)) {
    logging::debug << logging::MEDIUM
      << "correlation: retagging node (" << n->host_id << ", "
      << n->service_id << ") to unknown";
    hss.current_state = unknown_state(*n);
  }

  if (n->state != hss.current_state) {
    time_t now(time(NULL));

    // Update states.
    logging::debug << logging::MEDIUM
      << "correlation: node (" << n->host_id << ", " << n->service_id
      << ") changed status from " << n->state
      << " to " << hss.current_state;
    {
      // Old state.
      std::auto_ptr<state> state_update(
        is_host ? static_cast<state*>(new host_state)
                : static_cast<state*>(new service_state));
      state_update->current_state = n->state;
      state_update->end_time = now;
      state_update->host_id = n->host_id;
      state_update->service_id = n->service_id;
      state_update->start_time = n->since;
      _events.push_back(QSharedPointer<io::data>(state_update.get()));
      state_update.release();

      // Update node.
      n->since = now;
      n->state = hss.current_state;

      // New state.
      state_update.reset(
        is_host ? static_cast<state*>(new host_state)
                : static_cast<state*>(new service_state));
      state_update->current_state = n->state;
      state_update->end_time = 0;
      state_update->host_id = n->host_id;
      state_update->service_id = n->service_id;
      state_update->start_time = n->since;
      _events.push_back(QSharedPointer<io::data>(state_update.get()));
      state_update.release();
    }

    if (n->my_issue) {
      // Issue is over.
      if (!n->state) {
        // Debug message.
        logging::debug << logging::MEDIUM
          << "correlation: issue on node (" << n->host_id
          << ", " << n->service_id << ") is over";
        
        // Issue parenting deletion.
        for (QList<node*>::iterator it = n->depends_on.begin(),
               end = n->depends_on.end();
             it != end;
             ++it)
          if ((*it)->my_issue) {
            logging::debug << logging::LOW << "correlation: deleting " \
                 "issue parenting between dependent node ("
              << n->host_id << ", " << n->service_id << ") and node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            std::auto_ptr<issue_parent> p(new issue_parent);
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
            _events.push_back(QSharedPointer<io::data>(p.get()));
            p.release();
          }
        for (QList<node*>::iterator it = n->depended_by.begin(),
               end = n->depended_by.end();
             it != end;
             ++it)
          if ((*it)->my_issue) {
            logging::debug << logging::LOW << "correlation: deleting " \
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and dependent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            std::auto_ptr<issue_parent> p(new issue_parent);
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
            _events.push_back(QSharedPointer<io::data>(p.get()));
            p.release();
          }
        bool all_parents = true;
        for (QList<node*>::iterator it = n->parents.begin(),
               end = n->parents.end();
             it != end;
             ++it)
          if (!(*it)->my_issue)
            all_parents = false;
        if (all_parents)
          for (QList<node*>::iterator it = n->parents.begin(),
                 end = n->parents.end();
               it != end;
               ++it) {
            logging::debug << logging::LOW << "correlation: deleting " \
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and parent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            std::auto_ptr<issue_parent> p(new issue_parent);
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
            _events.push_back(QSharedPointer<io::data>(p.get()));
            p.release();
          }
        for (QList<node*>::iterator it = n->children.begin(),
               end = n->children.end();
             it != end;
             ++it) {
          if (!(*it)->my_issue.isNull()) {
            // Check that all parents of the node have an issue.
            all_parents = true;
            for (QList<node*>::iterator it2 = (*it)->parents.begin(),
                   end2 = (*it)->parents.end();
                 it2 != end2;
                 ++it2)
              all_parents = (all_parents && !(*it2)->my_issue.isNull());
            if (all_parents)
              for (QList<node*>::iterator it2 = (*it)->parents.begin(),
                     end2 = (*it)->parents.end();
                   it2 != end2;
                   ++it2) {
                logging::debug << logging::LOW << "correlation: " \
                     "deleting issue parenting between node ("
                  << (*it)->host_id << ", " << (*it)->service_id
                  << ") and parent node (" << (*it2)->host_id << ", "
                  << (*it2)->service_id << ")";
                std::auto_ptr<issue_parent> p(new issue_parent);
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
                _events.push_back(QSharedPointer<io::data>(p.get()));
                p.release();
              }
          }
        }

        // Terminate issue.
        n->my_issue->end_time = now;
        _events.push_back(QSharedPointer<io::data>(n->my_issue));
        n->my_issue.clear();
      }
    }
    else {
      // Set issue.
      n->my_issue = QSharedPointer<issue>(new issue);
      n->my_issue->host_id = n->host_id;
      n->my_issue->service_id = n->service_id;
      n->my_issue->start_time = now;

      // Store issue.
      {
        std::auto_ptr<issue> i(new issue(*(n->my_issue)));
        _events.push_back(QSharedPointer<io::data>(i.get()));
        i.release();
      }

      // Declare parenting.
      if (unknown_state(*n) == n->state) {
        // Loop dependencies.
        for (QList<node*>::iterator it = n->depends_on.begin(),
               end = n->depends_on.end();
             it != end;
             ++it)
          if ((*it)->my_issue) {
            logging::debug << logging::LOW << "correlation: creating " \
                 "issue parenting between dependent node ("
              << n->host_id << ", " << n->service_id << ") and node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            std::auto_ptr<issue_parent> parenting(new issue_parent);
            parenting->child_host_id = n->host_id;
            parenting->child_service_id = n->service_id;
            parenting->child_start_time = n->my_issue->start_time;
            parenting->parent_host_id = (*it)->host_id;
            parenting->parent_service_id = (*it)->service_id;
            parenting->parent_start_time = (*it)->my_issue->start_time;
            parenting->start_time = n->my_issue->start_time;
            _events.push_back(QSharedPointer<io::data>(parenting.get()));
            parenting.release();
          }

        // Loop parents.
        bool all_parent_issue = true;
        for (QList<node*>::iterator it = n->parents.begin(),
               end = n->parents.end();
             it != end;
             ++it)
          all_parent_issue = (all_parent_issue && (*it)->my_issue);
        if (all_parent_issue) {
          for (QList<node*>::iterator it = n->parents.begin(),
                 end = n->parents.end();
               it != end;
               ++it) {
            logging::debug << logging::LOW << "correlation: creating " \
                 "issue parenting between node (" << n->host_id << ", "
              << n->service_id << ") and parent node ("
              << (*it)->host_id << ", " << (*it)->service_id << ")";
            std::auto_ptr<issue_parent> parenting(new issue_parent);
            parenting->child_host_id = n->host_id;
            parenting->child_service_id = n->service_id;
            parenting->child_start_time = n->my_issue->start_time;
            parenting->parent_host_id = (*it)->host_id;
            parenting->parent_service_id = (*it)->service_id;
            parenting->parent_start_time = (*it)->my_issue->start_time;
            parenting->start_time = n->my_issue->start_time;
            _events.push_back(QSharedPointer<io::data>(parenting.get()));
            parenting.release();
          }
        }
      }

      // Loop dependant nodes.
      for (QList<node*>::iterator it = n->depended_by.begin(),
             end = n->depended_by.end();
           it != end;
           ++it)
        if (!(*it)->my_issue.isNull()) {
          logging::debug << logging::LOW << "correlation: creating " \
               "issue parenting between node (" << n->host_id << ", "
            << n->service_id << ") and dependent node ("
            << (*it)->host_id << ", " << (*it)->service_id << ")";
          std::auto_ptr<issue_parent> parenting(new issue_parent);
          parenting->child_host_id = (*it)->host_id;
          parenting->child_service_id = (*it)->service_id;
          parenting->child_start_time = (*it)->my_issue->start_time;
          parenting->parent_host_id = n->host_id;
          parenting->parent_service_id = n->service_id;
          parenting->parent_start_time = n->my_issue->start_time;
          parenting->start_time = n->my_issue->start_time;
          _events.push_back(QSharedPointer<io::data>(parenting.get()));
          parenting.release();
        }
                
      // Loop children.
      for (QList<node*>::iterator it = n->children.begin(),
             end = n->children.end();
           it != end;
           ++it)
        if (!(*it)->my_issue.isNull()) {
          // Check that all parents of the node have an issue.
          bool all_parent_issue = true;
          for (QList<node*>::iterator it2 = (*it)->parents.begin(),
                 end2 = (*it)->parents.end();
               it2 != end2;
               ++it2)
            all_parent_issue = (all_parent_issue && !(*it2)->my_issue.isNull());

          if (all_parent_issue)
            for (QList<node*>::iterator it2 = (*it)->parents.begin(),
                   end2 = (*it)->parents.end();
                 it2 != end2;
                 ++it2) {
              logging::debug << logging::LOW << "correlation: " \
                   "creating issue parenting between node ("
                << (*it)->host_id << ", " << (*it)->service_id
                << ") and parent node (" << (*it2)->host_id << ", "
                << (*it2)->service_id << ")";
              std::auto_ptr<issue_parent> parenting(new issue_parent);
              parenting->child_host_id = (*it)->host_id;
              parenting->child_service_id = (*it)->service_id;
              parenting->child_start_time = (*it)->my_issue->start_time;
              parenting->parent_host_id = (*it2)->host_id;
              parenting->parent_service_id = (*it2)->service_id;
              parenting->parent_start_time = (*it2)->my_issue->start_time;
              parenting->start_time = (*it2)->my_issue->start_time;
            }
        }
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

  if (l->service_id) {
    std::map<std::pair<unsigned int, unsigned int>, node>::iterator it(
      _services.find(std::make_pair(l->host_id, l->service_id)));
    if (it != _services.end())
      n = &it->second;
    else
      n = NULL;
  }
  else {
    std::map<unsigned int, node>::iterator it(_hosts.find(l->host_id));
    if (it != _hosts.end())
      n = &it->second;
    else
      n = NULL;
  }
  if (n && n->state) {
    QSharedPointer<issue> isu(_find_related_issue(*n));
    if (!isu.isNull())
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
QSharedPointer<issue> correlator::_find_related_issue(node& n) {
  QSharedPointer<issue> isu;

  if (n.state && !n.my_issue.isNull())
    isu = n.my_issue;
  else {
    isu.clear();
    for (QList<node*>::iterator it = n.depends_on.begin(),
           end = n.depends_on.end();
         it != end;
         ++it)
      if ((*it)->state) {
        isu = _find_related_issue(**it);
        break ;
      }
    if (isu.isNull()) {
      for (QList<node*>::iterator it = n.parents.begin(),
             end = n.parents.end();
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
  _hosts = c._hosts;
  _services = c._services;
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
correlator::correlator(correlator const& c) : io::stream(c) {
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
  io::stream::operator=(c);
  _internal_copy(c);
  return (*this);
}

/**
 *  Load a correlation file.
 *
 *  @param[in] correlation_file Path to a file containing host and
 *                              service relationships.
 */
void correlator::load(QString const& correlation_file) {
  parser p;
  p.parse(correlation_file, _hosts, _services);
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
 *  Treat a new event.
 *
 *  @param[inout] e Event to process.
 */
void correlator::write(QSharedPointer<io::data> e) {
  try {
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
  }
  catch (exceptions::msg const& e) {
    logging::error << logging::HIGH << e.what();
  }
  return ;
}
