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
#include "correlation/parser.h"
#include "events/host.h"
#include "events/issue.h"
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
    &Correlator::CorrelateNothing,    // UNKNOWN
    &Correlator::CorrelateNothing,    // ACKNOWLEDGEMENT
    &Correlator::CorrelateNothing,    // COMMENT
    &Correlator::CorrelateNothing,    // DOWNTIME
    &Correlator::CorrelateNothing,    // HOST
    &Correlator::CorrelateNothing,    // HOSTCHECK
    &Correlator::CorrelateNothing,    // HOSTDEPENDENCY
    &Correlator::CorrelateNothing,    // HOSTGROUP
    &Correlator::CorrelateNothing,    // HOSTGROUPMEMBER
    &Correlator::CorrelateNothing,    // HOSTPARENT
    &Correlator::CorrelateHostStatus, // HOSTSTATUS
    &Correlator::CorrelateNothing,    // ISSUE
    &Correlator::CorrelateNothing,    // ISSUESTATUS
    &Correlator::CorrelateNothing,    // LOG
    &Correlator::CorrelateNothing,    // PROGRAM
    &Correlator::CorrelateNothing,    // PROGRAMSTATUS
    &Correlator::CorrelateNothing,    // SERVICE
    &Correlator::CorrelateNothing,    // SERVICECHECK
    &Correlator::CorrelateNothing,    // SERVICEDEPENDENCY
    &Correlator::CorrelateNothing,    // SERVICEGROUP
    &Correlator::CorrelateNothing,    // SERVICEGROUPMEMBER
    &Correlator::CorrelateNothing,    // SERVICESTATUS
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

  all_parents_down = true;
  one_dependency_down = false;
  for (std::list<Node*>::const_iterator it = node.parents.begin(),
         end = node.parents.end();
       it != end;
       ++it)
    all_parents_down = (all_parents_down && (*it)->state);
  if (!all_parents_down)
    {
      for (std::list<Node*>::const_iterator it = node.depends_on.begin(),
             end = node.depends_on.end();
           it != end;
           ++it)
        one_dependency_down = (one_dependency_down || (*it)->state);
    }
  return (all_parents_down || one_dependency_down);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

void Correlator::CorrelateHostStatus(Events::Event& event)
{
  Events::HostStatus& hs(*static_cast<Events::HostStatus*>(&event));
  std::map<int, Node>::iterator host_it;

  // Find host in host list.
  if ((host_it = this->hosts_.find(hs.id)) == this->hosts_.end())
    throw (Exception(0, "Invalid host status provided."));

  Node& host(host_it->second);

  if (host.state != hs.current_state)
    {
      if (hs.current_state)
        {
          Events::Issue* issue;

          // Check if unknown flag should be set.
          if (ShouldBeUnknown(host))
            {
              hs.current_state = 3; // UNKNOWN
              issue = this->FindRelatedIssue(host);
            }
          else
            {
              // Set issue.
              host.issue.reset(new Events::Issue);
              (*host.issue) << hs;
              host.issue->start_time = time(NULL);

              // Store issue. (XXX : not safe)
              this->events_.push_back(new Events::Issue(*(host.issue.get())));

              // Get current issue.
              issue = host.issue.get();
            }
          // XXX : loop depended_by to fetch their issues
          // XXX : loop childs to fetch their issues
        }
      else
        {
          // Issue is over.
          if (host.issue.get())
            {
              (*host.issue) << hs;
              host.issue->end_time = time(NULL);
              this->events_.push_back(host.issue.get());
              host.issue.release();
            }

          // Restore possible childs. If another failure occurs, this will be
          // another issue.
          host.state = hs.current_state;
          for (std::list<Node*>::iterator it = host.children.begin(),
                 end = host.children.end();
               it != end;
               ++it)
            if (!ShouldBeUnknown(**it))
              (*it)->state = 0;
          for (std::list<Node*>::iterator it = host.depended_by.begin(),
                 end = host.depended_by.end();
               it != end;
               ++it)
            if (!ShouldBeUnknown(**it))
              (*it)->state = 0;
        }
    }
  host.state = hs.current_state;
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
 *  Browse the parenting tree of the given node and find its ancestor's issue
 *  which causes it to be in undetermined state.
 *
 *  \param[in] node Base node.
 *
 *  \return The issue associated with node.
 */
Events::Issue* Correlator::FindRelatedIssue(Node& node)
{
  Node* related;

  related = NULL;
  for (std::list<Node*>::iterator it = node.depends_on.begin(),
         end = node.depends_on.end();
       it != end;
       ++it)
    if ((*it)->state)
      {
        related = *it;
        break ;
      }
  if (!related)
    related = *(node.parents.begin());
  assert(related->state);
  return (related->issue.get() ? related->issue.get()
                               : this->FindRelatedIssue(*related));
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
  Parser parser;

  parser.Parse(correlation_file, this->hosts_, this->services_);
  return ;
}
