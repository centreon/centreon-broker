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
#include "events/host.h"
#include "events/issue.h"
#include "exception.h"
#include "multiplexing/publisher.h"

using namespace Correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

void Correlator::CorrelateHost(Events::HostStatus& hs)
{
  // Find host in host list.
  std::map<int, Node>::iterator host_it;

  if ((host_it = this->hosts_.find(hs.id)) == this->hosts_.end())
    throw (Exception(0, "Invalid host status provided."));

  Node& host(host_it->second);

  if (host.state != hs.current_state)
    {
      if (hs.current_state)
	{
	  bool all_parents_down;
	  Events::Issue* issue;
	  std::list<Node*>::iterator it;
	  bool one_dependency_down;

	  all_parents_down = true;
	  one_dependency_down = false;
	  for (it = host.parents.begin(); it != host.parents.end(); ++it)
	    all_parents_down = (all_parents_down && (*it)->state);
	  if (!all_parents_down)
	    {
	      for (it = host.depends_on.begin();
		   it != host.depends_on.end();
		   ++it)
		one_dependency_down = (one_dependency_down || (*it)->state);
	    }
	  if (all_parents_down || one_dependency_down)
	    {
	      host.state = 3; // UNKNOWN
	      issue = this->FindRelatedIssue(host);
	    }
	  else
	    {
	      std::auto_ptr<Events::Issue> publish_issue;
	      Multiplexing::Publisher publisher;

	      // Set issue.
	      host.issue.reset(new Events::Issue);
	      (*host.issue) << hs;
	      host.issue->start_time = time(NULL);

	      // Publish issue.
	      publish_issue.reset(new Events::Issue(*(host.issue.get())));
	      publisher.Event(publish_issue.get());
	      publish_issue.release();

	      // Get current issue.
	      issue = host.issue.get();
	    }
	  // XXX : loop depended_by
	  // XXX : loop childs
	}
      else
	{
	  // issue is over
	  // recursively set childs
	}
    }
  else
    {
      // update status
    }
  return ;
}

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
Correlator::Correlator() {}

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
}
