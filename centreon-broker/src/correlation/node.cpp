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

#include "correlation/node.h"
#include "events/issue.h"

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
 *  \param[in] node Object to copy.
 */
void Node::InternalCopy(const Node& node)
{
  this->children    = node.children;
  this->depended_by = node.depended_by;
  this->depends_on  = node.depends_on;
  this->host_id     = node.host_id;
  if (node.issue.get())
    this->issue.reset(new Events::Issue(*(node.issue.get())));
  this->parents     = node.parents;
  this->service_id  = node.service_id;
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
Node::Node() {}

/**
 *  Copy constructor.
 *
 *  \param[in] node Object to copy.
 */
Node::Node(const Node& node)
{
  this->InternalCopy(node);
}

/**
 *  Destructor.
 */
Node::~Node() {}

/**
 *  Assignment operator.
 *
 *  \param[in] node Object to copy.
 *
 *  \return *this
 */
Node& Node::operator=(const Node& node)
{
  this->InternalCopy(node);
  return (*this);
}

/**
 *  Extract useful status fields from an host status.
 *
 *  \param[in] hs HostStatus to extract from.
 *
 *  \return *this
 */
Node& Node::operator<<(const Events::HostStatus& hs)
{
  this->host_id    = hs.id;
  this->service_id = 0;
  this->state      = hs.current_state;
  return (*this);
}
