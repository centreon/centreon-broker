/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <time.h>
#include "correlation/correlator.h"
#include "correlation/parser.hh"
#include "events/host.h"
#include "events/issue.h"
#include "events/issue_parent.h"
#include "events/service_status.h"
#include "exception.h"
#include "multiplexing/publisher.h"

using namespace Correlation;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Dispatch table.
void (Correlator::* Correlator::dispatch_table[])(Events::Event&) =
  {
    &Correlator::CorrelateNothing,      // UNKNOWN
    &Correlator::CorrelateNothing,      // ACKNOWLEDGEMENT
    &Correlator::CorrelateNothing,      // COMMENT
    &Correlator::CorrelateNothing,      // DOWNTIME
    &Correlator::CorrelateNothing,      // HOST
    &Correlator::CorrelateNothing,      // HOSTCHECK
    &Correlator::CorrelateNothing,      // HOSTDEPENDENCY
    &Correlator::CorrelateNothing,      // HOSTGROUP
    &Correlator::CorrelateNothing,      // HOSTGROUPMEMBER
    &Correlator::CorrelateNothing,      // HOSTPARENT
    &Correlator::CorrelateHostStatus,   // HOSTSTATUS
    &Correlator::CorrelateNothing,      // ISSUE
    &Correlator::CorrelateNothing,      // ISSUEUPDATE
    &Correlator::CorrelateLog,          // LOG
    &Correlator::CorrelateNothing,      // PROGRAM
    &Correlator::CorrelateNothing,      // PROGRAMSTATUS
    &Correlator::CorrelateNothing,      // SERVICE
    &Correlator::CorrelateNothing,      // SERVICECHECK
    &Correlator::CorrelateNothing,      // SERVICEDEPENDENCY
    &Correlator::CorrelateNothing,      // SERVICEGROUP
    &Correlator::CorrelateNothing,      // SERVICEGROUPMEMBER
    &Correlator::CorrelateServiceStatus // SERVICESTATUS
  };

/**
 *  Determine whether or not a node should have the unknown state.
 *
 *  \param[in] node Node to check.
 *
 *  \return true if the node should be unknown.
 */
static bool ShouldBeUnknown(const Node& node)
{
  bool all_parents_down;
  bool one_dependency_down;

  // If node has no parents, then all_parents_down will be false.
  if (node.parents.size())
    {
      all_parents_down = true;
      for (std::list<Node*>::const_iterator it = node.parents.begin(),
             end = node.parents.end();
           it != end;
           ++it)
        all_parents_down = (all_parents_down && (*it)->state);
    }
  else
    all_parents_down = false;

  // Check dependencies.
  one_dependency_down = false;
  for (std::list<Node*>::const_iterator it = node.depends_on.begin(),
         end = node.depends_on.end();
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
 *  Process a HostStatus or ServiceStatus event.
 *
 *  \param[in] event   Event to process.
 *  \param[in] is_host true if the event is a HostStatus.
 */
void Correlator::CorrelateHostServiceStatus(Events::Event& event, bool is_host)
{
  Events::HostServiceStatus& hss(
    *static_cast<Events::HostServiceStatus*>(&event));
  Node* node;

  // Find node in appropriate list.
  if (is_host)
    {
      std::map<int, Node>::iterator hs_it;
      Events::HostStatus& hs(*static_cast<Events::HostStatus*>(&event));

      if ((hs_it = this->hosts_.find(hs.host_id)) == this->hosts_.end())
        throw (Exception(0, "Invalid host status provided."));
      node = &hs_it->second;
    }
  else
    {
      std::map<std::pair<int, int>, Node>::iterator ss_it;
      Events::ServiceStatus& ss(*static_cast<Events::ServiceStatus*>(&event));

      if ((ss_it = this->services_.find(std::make_pair(ss.host_id, ss.service_id)))
          == this->services_.end())
        throw (Exception(0, "Invalid service status provided."));
      node = &ss_it->second;
    }

  if (hss.current_state && (hss.current_state != 3) && ShouldBeUnknown(*node))
    hss.current_state = 3;

  if (node->state != hss.current_state)
    {
      if (node->issue)
        {
          // Issue is over.
          if (!hss.current_state)
            {
              node->issue->end_time = time(NULL);
              this->events_.push_back(node->issue);
              node->issue = NULL;
              node->state = 0;
            }
        }
      else
        {
          // Set issue.
          node->issue = new Events::Issue;
          node->issue->host_id = node->host_id;
          node->issue->service_id = node->service_id;
          node->issue->start_time = time(NULL);

          // Store issue. (XXX : not safe)
          this->events_.push_back(new Events::Issue(*(node->issue)));

          // Update state.
          node->state = hss.current_state;

          // Declare parenting.
          if (3 == node->state)
            {
              // Loop dependencies.
              for (std::list<Node*>::iterator it = node->depends_on.begin(),
                     end = node->depends_on.end();
                   it != end;
                   ++it)
                if ((*it)->issue)
                  {
                    std::auto_ptr<Events::IssueParent> parenting;

                    parenting.reset(new Events::IssueParent);
                    parenting->child_host_id = node->host_id;
                    parenting->child_service_id = node->service_id;
                    parenting->child_start_time = node->issue->start_time;
                    parenting->parent_host_id = (*it)->host_id;
                    parenting->parent_service_id = (*it)->service_id;
                    parenting->parent_start_time = (*it)->issue->start_time;
                    parenting->start_time = node->issue->start_time;
                    this->events_.push_back(parenting.get());
                    parenting.release();
                  }

              // Loop parents.
              bool all_parent_issue = true;
              for (std::list<Node*>::iterator it = node->parents.begin(),
                     end = node->parents.end();
                   it != end;
                   ++it)
                all_parent_issue = (all_parent_issue && (*it)->issue);
              if (all_parent_issue)
                {
                  for (std::list<Node*>::iterator it = node->parents.begin(),
                         end = node->parents.end();
                       it != end;
                       ++it)
                    {
                      std::auto_ptr<Events::IssueParent> parenting;

                      parenting.reset(new Events::IssueParent);
                      parenting->child_host_id = node->host_id;
                      parenting->child_service_id = node->service_id;
                      parenting->child_start_time = node->issue->start_time;
                      parenting->parent_host_id = (*it)->host_id;
                      parenting->parent_service_id = (*it)->service_id;
                      parenting->parent_start_time = (*it)->issue->start_time;
                      parenting->start_time = node->issue->start_time;
                      this->events_.push_back(parenting.get());
                      parenting.release();
                    }
                }
            }

          // Loop dependant nodes.
          for (std::list<Node*>::iterator it = node->depended_by.begin(),
                 end = node->depended_by.end();
               it != end;
               ++it)
            if ((*it)->issue)
              {
                std::auto_ptr<Events::IssueParent> parenting;

                parenting.reset(new Events::IssueParent);
                parenting->child_host_id = (*it)->host_id;
                parenting->child_service_id = (*it)->service_id;
                parenting->child_start_time = (*it)->issue->start_time;
                parenting->parent_host_id = node->host_id;
                parenting->parent_service_id = node->service_id;
                parenting->parent_start_time = node->issue->start_time;
                parenting->start_time = node->issue->start_time;
                this->events_.push_back(parenting.get());
                parenting.release();
              }
                
          // Loop children.
          for (std::list<Node*>::iterator it = node->children.begin(),
                 end = node->children.end();
               it != end;
               ++it)
            if ((*it)->issue)
              {
                // Check that all parents of the node have an issue.
                bool all_parent_issue = true;
                for (std::list<Node*>::iterator it2 = (*it)->parents.begin(),
                       end2 = (*it)->parents.end();
                     it2 != end2;
                     ++it2)
                  all_parent_issue = (all_parent_issue && (*it2)->issue);

                if (all_parent_issue)
                  for (std::list<Node*>::iterator it2 = (*it)->parents.begin(),
                         end2 = (*it)->parents.end();
                       it2 != end2;
                       ++it2)
                    {
                      std::auto_ptr<Events::IssueParent> parenting;

                      parenting.reset(new Events::IssueParent);
                      parenting->child_host_id = (*it2)->host_id;
                      parenting->child_service_id = (*it2)->service_id;
                      parenting->child_start_time = (*it2)->issue->start_time;
                      parenting->parent_host_id = node->host_id;
                      parenting->parent_service_id = node->service_id;
                      parenting->parent_start_time = node->issue->start_time;
                      parenting->start_time = node->issue->start_time;
                    }
              }
        }

      // Create new state.
      // XXX
    }
  return ;
}

/**
 *  Process a HostStatus event.
 *
 *  \param[in] event Event to process.
 */
void Correlator::CorrelateHostStatus(Events::Event& event)
{
  this->CorrelateHostServiceStatus(event, true);
  return ;
}

/**
 *  Process a Log event.
 */
void Correlator::CorrelateLog(Events::Event& event)
{
  Events::Log* log(static_cast<Events::Log*>(&event));
  Node* node;

  if (log->service_id)
    {
      std::map<std::pair<int, int>, Node>::iterator it;

      if ((it = this->services_.find(std::make_pair(log->host_id, log->service_id)))
          != this->services_.end())
        node = &it->second;
      else
        node = NULL;
    }
  else
    {
      std::map<int, Node>::iterator it;

      if ((it = this->hosts_.find(log->host_id)) != this->hosts_.end())
        node = &it->second;
      else
        node = NULL;
    }
  if (node && node->state)
    {
      Events::Issue* issue;

      issue = this->FindRelatedIssue(*node);
      if (issue)
        log->issue_start_time = issue->start_time;
    }
  return ;
}

/**
 *  Do nothing (callback called on untreated event types).
 *
 *  \param[in] event Unused.
 */
void Correlator::CorrelateNothing(Events::Event& event)
{
  (void)event;
  return ;
}

/**
 *  Process a ServiceStatus event.
 *
 *  \param[in] event Event to process.
 */
void Correlator::CorrelateServiceStatus(Events::Event& event)
{
  this->CorrelateHostServiceStatus(event, false);
  return ;
}

/**
 *  Browse the parenting tree of the given node and find its ancestor's issue
 *  which causes it to be in undetermined state.
 *
 *  \param[in] node Base node.
 *
 *  \return The issue associated with node.
 */
Events::Issue* Correlator::FindRelatedIssue(Node& node)
{
  Events::Issue* issue;

  if (node.state && node.issue)
    issue = node.issue;
  else
    {
      issue = NULL;
      for (std::list<Node*>::iterator it = node.depends_on.begin(),
             end = node.depends_on.end();
           it != end;
           ++it)
        if ((*it)->state)
          {
            issue = this->FindRelatedIssue(**it);
            break ;
          }
      if (!issue)
        {
          for (std::list<Node*>::iterator it = node.parents.begin(),
                 end = node.parents.end();
               it != end;
               ++it)
            if ((*it)->state)
              {
                issue = this->FindRelatedIssue(**it);
                break ;
              }
        }
    }
  return (issue);
}

/**
 *  \brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] correlator Object to copy.
 */
void Correlator::InternalCopy(const Correlator& correlator)
{
  this->hosts_    = correlator.hosts_;
  this->services_ = correlator.services_;
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
Correlator::Correlator()
{
  assert((sizeof(dispatch_table) / sizeof(*dispatch_table))
         == Events::Event::EVENT_TYPES_NB);
}

/**
 *  Copy constructor.
 *
 *  \param[in] correlator Object to copy.
 */
Correlator::Correlator(const Correlator& correlator)
{
  this->InternalCopy(correlator);
}

/**
 *  Destructor.
 */
Correlator::~Correlator() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] correlator Object to copy.
 *
 *  \return *this
 */
Correlator& Correlator::operator=(const Correlator& correlator)
{
  this->InternalCopy(correlator);
  return (*this);
}

/**
 *  Treat a new event.
 *
 *  \param[inout] event Event to process.
 */
void Correlator::Event(Events::Event& event)
{
  (this->*dispatch_table[event.GetType()])(event);
  return ;
}

/**
 *  Get the next available correlated event.
 *
 *  \return The next available correlated event.
 */
Events::Event* Correlator::Event()
{
  Events::Event* event;

  if (this->events_.empty())
    event = NULL;
  else
    {
      event = this->events_.front();
      this->events_.pop_front();
    }
  return (event);
}

/**
 *  Load a correlation file.
 *
 *  \param[in] correlation_file Path to a file containing host and service
 *                              relationships.
 */
void Correlator::Load(const char* correlation_file)
{
  parser p;

  p.parse(correlation_file, this->hosts_, this->services_);
  return ;
}
