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
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <time.h>
#include "correlation/correlator.hh"
#include "correlation/parser.hh"
#include "events/host.hh"
#include "events/issue.hh"
#include "events/issue_parent.hh"
#include "events/service_status.hh"
#include "events/state.hh"
#include "exceptions/basic.hh"
#include "multiplexing/publisher.hh"

using namespace correlation;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Dispatch table.
void (correlator::* correlator::_dispatch_table[])(events::event&) = {
  &correlator::_correlate_nothing,        // UNKNOWN
  &correlator::_correlate_nothing,        // ACKNOWLEDGEMENT
  &correlator::_correlate_nothing,        // COMMENT
  &correlator::_correlate_nothing,        // CUSTOMVARIABLE
  &correlator::_correlate_nothing,        // CUSTOMVARIABLESTATUS
  &correlator::_correlate_nothing,        // DOWNTIME
  &correlator::_correlate_nothing,        // EVENTHANDLER
  &correlator::_correlate_nothing,        // FLAPPINGSTATUS
  &correlator::_correlate_nothing,        // HOST
  &correlator::_correlate_nothing,        // HOSTCHECK
  &correlator::_correlate_nothing,        // HOSTDEPENDENCY
  &correlator::_correlate_nothing,        // HOSTGROUP
  &correlator::_correlate_nothing,        // HOSTGROUPMEMBER
  &correlator::_correlate_nothing,        // HOSTPARENT
  &correlator::_correlate_host_status,    // HOSTSTATUS
  &correlator::_correlate_nothing,        // ISSUE
  &correlator::_correlate_nothing,        // ISSUEPARENT
  &correlator::_correlate_log,            // LOG
  &correlator::_correlate_nothing,        // NOTIFICATION
  &correlator::_correlate_nothing,        // PROGRAM
  &correlator::_correlate_nothing,        // PROGRAMSTATUS
  &correlator::_correlate_nothing,        // SERVICE
  &correlator::_correlate_nothing,        // SERVICECHECK
  &correlator::_correlate_nothing,        // SERVICEDEPENDENCY
  &correlator::_correlate_nothing,        // SERVICEGROUP
  &correlator::_correlate_nothing,        // SERVICEGROUPMEMBER
  &correlator::_correlate_service_status, // SERVICESTATUS
  &correlator::_correlate_nothing         // STATE
};

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
    for (std::list<node*>::const_iterator it = n.parents.begin(),
           end = n.parents.end();
         it != end;
         ++it)
      all_parents_down = (all_parents_down && (*it)->state);
  }
  else
    all_parents_down = false;

  // Check dependencies.
  one_dependency_down = false;
  for (std::list<node*>::const_iterator it = n.depends_on.begin(),
         end = n.depends_on.end();
       it != end;
       ++it)
    one_dependency_down = (one_dependency_down || (*it)->state);

  return (all_parents_down || one_dependency_down);
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
void correlator::_correlate_host_service_status(events::event& e,
                                                bool is_host) {
  events::host_service_status& hss(
    *static_cast<events::host_service_status*>(&e));
  node* n;

  // Find node in appropriate list.
  if (is_host) {
    events::host_status& hs(*static_cast<events::host_status*>(&e));
    std::map<int, node>::iterator hs_it(_hosts.find(hs.host_id));
    if (hs_it == _hosts.end())
      throw (exceptions::basic() << "invalid host status provided");
    n = &hs_it->second;
  }
  else {
    events::service_status& ss(*static_cast<events::service_status*>(&e));
    std::map<std::pair<int, int>, node>::iterator ss_it(
      _services.find(std::make_pair(ss.host_id, ss.service_id)));
    if (ss_it == _services.end())
      throw (exceptions::basic() << "invalid service status provided");
    n = &ss_it->second;
  }

  if (hss.current_state
      && (hss.current_state != 3)
      && should_be_unknown(*n))
    hss.current_state = 3;

  if (n->state != hss.current_state) {
    time_t now(time(NULL));

    // Update states.
    {
      // Old state.
      std::auto_ptr<events::state> state_update(new events::state);
      state_update->current_state = n->state;
      state_update->end_time = now;
      state_update->host_id = n->host_id;
      state_update->service_id = n->service_id;
      state_update->start_time = n->since;
      _events.push_back(state_update.get());
      state_update.release();

      // Update node.
      n->since = now;
      n->state = hss.current_state;

      // New state.
      state_update.reset(new events::state);
      state_update->current_state = n->state;
      state_update->end_time = 0;
      state_update->host_id = n->host_id;
      state_update->service_id = n->service_id;
      state_update->start_time = n->since;
      _events.push_back(state_update.get());
      state_update.release();
    }

    if (n->issue) {
      // Issue is over.
      if (!n->state) {
        // Issue parenting deletion.
        for (std::list<node*>::iterator it = n->depends_on.begin(),
               end = n->depends_on.end();
             it != end;
             ++it)
          if ((*it)->issue) {
            std::auto_ptr<events::issue_parent> p(
              new events::issue_parent);
            p->child_host_id = n->host_id;
            p->child_service_id = n->service_id;
            p->child_start_time = n->issue->start_time;
            p->parent_host_id = (*it)->host_id;
            p->parent_service_id = (*it)->service_id;
            p->parent_start_time = (*it)->issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.get());
            p.release();
          }
        for (std::list<node*>::iterator it = n->depended_by.begin(),
               end = n->depended_by.end();
             it != end;
             ++it)
          if ((*it)->issue) {
            std::auto_ptr<events::issue_parent> p(
              new events::issue_parent);
            p->child_host_id = (*it)->host_id;
            p->child_service_id = (*it)->service_id;
            p->child_start_time = (*it)->issue->start_time;
            p->parent_host_id = n->host_id;
            p->parent_service_id = n->service_id;
            p->parent_start_time = n->issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.get());
            p.release();
          }
        bool all_parents = true;
        for (std::list<node*>::iterator it = n->parents.begin(),
               end = n->parents.end();
             it != end;
             ++it)
          if (!(*it)->issue)
            all_parents = false;
        if (all_parents)
          for (std::list<node*>::iterator it = n->parents.begin(),
                 end = n->parents.end();
               it != end;
               ++it) {
            std::auto_ptr<events::issue_parent> p(
              new events::issue_parent);
            p->child_host_id = n->host_id;
            p->child_service_id = n->service_id;
            p->child_start_time = n->issue->start_time;
            p->parent_host_id = (*it)->host_id;
            p->parent_service_id = (*it)->service_id;
            p->parent_start_time = (*it)->issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.get());
            p.release();
          }
        all_parents = true;
        for (std::list<node*>::iterator it = n->children.begin(),
               end = n->children.end();
             it != end;
             ++it)
          if (!(*it)->issue)
            all_parents = false;
        if (all_parents)
          for (std::list<node*>::iterator it = n->children.begin(),
                 end = n->children.end();
               it != end;
               ++it) {
            std::auto_ptr<events::issue_parent> p(
              new events::issue_parent);
            p->child_host_id = (*it)->host_id;
            p->child_service_id = (*it)->service_id;
            p->child_start_time = (*it)->issue->start_time;
            p->parent_host_id = n->host_id;
            p->parent_service_id = n->service_id;
            p->parent_start_time = n->issue->start_time;
            p->start_time = (p->child_start_time > p->parent_start_time
                             ? p->child_start_time
                             : p->parent_start_time);
            p->end_time = now;
            _events.push_back(p.get());
            p.release();
          }

        // Terminate issue.
        n->issue->end_time = now;
        _events.push_back(n->issue);
        n->issue = NULL;
      }
    }
    else {
      // Set issue.
      n->issue = new events::issue;
      n->issue->host_id = n->host_id;
      n->issue->service_id = n->service_id;
      n->issue->start_time = now;

      // Store issue.
      {
        std::auto_ptr<events::issue> i(new events::issue(*(n->issue)));
        _events.push_back(i.get());
        i.release();
      }

      // Declare parenting.
      if (3 == n->state) {
        // Loop dependencies.
        for (std::list<node*>::iterator it = n->depends_on.begin(),
               end = n->depends_on.end();
             it != end;
             ++it)
          if ((*it)->issue) {
            std::auto_ptr<events::issue_parent> parenting(
              new events::issue_parent);
            parenting->child_host_id = n->host_id;
            parenting->child_service_id = n->service_id;
            parenting->child_start_time = n->issue->start_time;
            parenting->parent_host_id = (*it)->host_id;
            parenting->parent_service_id = (*it)->service_id;
            parenting->parent_start_time = (*it)->issue->start_time;
            parenting->start_time = n->issue->start_time;
            _events.push_back(parenting.get());
            parenting.release();
          }

        // Loop parents.
        bool all_parent_issue = true;
        for (std::list<node*>::iterator it = n->parents.begin(),
               end = n->parents.end();
             it != end;
             ++it)
          all_parent_issue = (all_parent_issue && (*it)->issue);
        if (all_parent_issue) {
          for (std::list<node*>::iterator it = n->parents.begin(),
                 end = n->parents.end();
               it != end;
               ++it) {
            std::auto_ptr<events::issue_parent> parenting(
              new events::issue_parent);
            parenting->child_host_id = n->host_id;
            parenting->child_service_id = n->service_id;
            parenting->child_start_time = n->issue->start_time;
            parenting->parent_host_id = (*it)->host_id;
            parenting->parent_service_id = (*it)->service_id;
            parenting->parent_start_time = (*it)->issue->start_time;
            parenting->start_time = n->issue->start_time;
            _events.push_back(parenting.get());
            parenting.release();
          }
        }
      }

      // Loop dependant nodes.
      for (std::list<node*>::iterator it = n->depended_by.begin(),
             end = n->depended_by.end();
           it != end;
           ++it)
        if ((*it)->issue) {
          std::auto_ptr<events::issue_parent> parenting(
            new events::issue_parent);
          parenting->child_host_id = (*it)->host_id;
          parenting->child_service_id = (*it)->service_id;
          parenting->child_start_time = (*it)->issue->start_time;
          parenting->parent_host_id = n->host_id;
          parenting->parent_service_id = n->service_id;
          parenting->parent_start_time = n->issue->start_time;
          parenting->start_time = n->issue->start_time;
          _events.push_back(parenting.get());
          parenting.release();
        }
                
      // Loop children.
      for (std::list<node*>::iterator it = n->children.begin(),
             end = n->children.end();
           it != end;
           ++it)
        if ((*it)->issue) {
          // Check that all parents of the node have an issue.
          bool all_parent_issue = true;
          for (std::list<node*>::iterator it2 = (*it)->parents.begin(),
                 end2 = (*it)->parents.end();
               it2 != end2;
               ++it2)
            all_parent_issue = (all_parent_issue && (*it2)->issue);

          if (all_parent_issue)
            for (std::list<node*>::iterator it2 = (*it)->parents.begin(),
                   end2 = (*it)->parents.end();
                 it2 != end2;
                 ++it2) {
              std::auto_ptr<events::issue_parent> parenting(
                new events::issue_parent);
              parenting->child_host_id = (*it2)->host_id;
              parenting->child_service_id = (*it2)->service_id;
              parenting->child_start_time = (*it2)->issue->start_time;
              parenting->parent_host_id = n->host_id;
              parenting->parent_service_id = n->service_id;
              parenting->parent_start_time = n->issue->start_time;
              parenting->start_time = n->issue->start_time;
            }
        }
    }
  }
  return ;
}

/**
 *  Process a host_status event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_host_status(events::event& e) {
  _correlate_host_service_status(e, true);
  return ;
}

/**
 *  Process a log_entry event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_log(events::event& e) {
  events::log_entry* l(static_cast<events::log_entry*>(&e));
  node* n;

  if (l->service_id) {
    std::map<std::pair<int, int>, node>::iterator it(
      _services.find(std::make_pair(l->host_id, l->service_id)));
    if (it != _services.end())
      n = &it->second;
    else
      n = NULL;
  }
  else {
    std::map<int, node>::iterator it(_hosts.find(l->host_id));
    if (it != _hosts.end())
      n = &it->second;
    else
      n = NULL;
  }
  if (n && n->state) {
    events::issue* issue(_find_related_issue(*n));
    if (issue)
      l->issue_start_time = issue->start_time;
  }
  return ;
}

/**
 *  Do nothing (callback called on untreated event types).
 *
 *  @param[in] et Unused.
 */
void correlator::_correlate_nothing(events::event& e) {
  (void)e;
  return ;
}

/**
 *  Process a service_status event.
 *
 *  @param[in] e Event to process.
 */
void correlator::_correlate_service_status(events::event& e) {
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
events::issue* correlator::_find_related_issue(node& n) {
  events::issue* issue;

  if (n.state && n.issue)
    issue = n.issue;
  else {
    issue = NULL;
    for (std::list<node*>::iterator it = n.depends_on.begin(),
           end = n.depends_on.end();
         it != end;
         ++it)
      if ((*it)->state) {
        issue = _find_related_issue(**it);
        break ;
      }
    if (!issue) {
      for (std::list<node*>::iterator it = n.parents.begin(),
             end = n.parents.end();
           it != end;
           ++it)
        if ((*it)->state) {
          issue = _find_related_issue(**it);
          break ;
        }
    }
  }
  return (issue);
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
correlator::correlator() {
  assert((sizeof(_dispatch_table) / sizeof(*_dispatch_table))
         == events::event::EVENT_TYPES_NB);
}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
correlator::correlator(correlator const& c) {
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
  _internal_copy(c);
  return (*this);
}

/**
 *  Treat a new event.
 *
 *  @param[inout] e Event to process.
 */
void correlator::event(events::event& e) {
  (this->*_dispatch_table[e.get_type()])(e);
  return ;
}

/**
 *  Get the next available correlated event.
 *
 *  @return The next available correlated event.
 */
events::event* correlator::event() {
  events::event* e;

  if (_events.empty())
    e = NULL;
  else {
    e = _events.front();
    _events.pop_front();
  }
  return (e);
}

/**
 *  Load a correlation file.
 *
 *  @param[in] correlation_file Path to a file containing host and
 *                              service relationships.
 */
void correlator::load(char const* correlation_file) {
  parser p;
  p.parse(correlation_file, _hosts, _services);
  return ;
}
