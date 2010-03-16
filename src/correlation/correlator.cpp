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

#include "correlation/correlator.h"
#include "events/host.h"

using namespace Correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

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
  this->issues_   = correlator.issues_;
  this->services_ = correlator.services_;
  return ;
}

void Correlator::CorrelateHost(Events::Host& host)
{
  if (host.current_state)
    {
      std::map<int, Node>::iterator host_it;

      host_it = this->hosts_.find(host.id);
      if (host_it != this->hosts_.end())
	{
	  // Issue is self-assigned. Updated it.
	  if (host_it->second.issue.get())
	    ;
	  else
	    {
	      bool all_parents_down;
	      std::list<Node*>::const_iterator parent_it;

	      all_parents_down = true;
	      for (parent_it = host_it->second.parents.begin();
		   parent_it != host_it->second.parents.end();
		   ++parent_it)
		all_parents_down = (all_parents_down
                                    && (*parent_it)->state);
	      // Issue comes from our parents. Our status is UNKNOWN.
	      if (all_parents_down)
		host.current_state = 3;
	      else
		{
		  bool dependency_down;
		  std::list<Node*>::const_iterator dependency_it;

		  dependency_down = false;
		  for (dependency_it = host_it->second.depends_on.begin();
		       dependency_it != host_it->second.depends_on.end();
		       ++dependency_it)
		    dependency_down = (dependency_down || (*dependency_it)->state);
		  // Issue comes from a dependency. Our status is UNKNOWN.
		  if (dependency_down)
		    host.current_state = 3;
		}
	    }
	}
      // Check children.
    }
  else
    {
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
