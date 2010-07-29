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

#include <stdlib.h>
#include <string.h>
#include "correlation/parser.h"
#include "exception.h"

using namespace Correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Find a node with its host_id and optionally its service_id.
 *
 *  @param[in] host_id    String of the host_id.
 *  @param[in] service_id Potentially NULL string of the service_id.
 *
 *  @return Node matching host_id/service_id.
 */
Node* Parser::FindNode(const char* host_id, const char* service_id)
{
  Node* node;

  node = NULL;
  if (!service_id)
    {
      std::map<int, Node>::iterator it;

      it = this->hosts_->find(strtol(host_id, NULL, 0));
      if (it != this->hosts_->end())
	node = &it->second;
    }
  else
    {
      std::map<std::pair<int, int>, Node>::iterator it;

      it = this->services_->find(std::make_pair(strtol(host_id, NULL, 0),
					       strtol(service_id, NULL, 0)));
      if (it != this->services_->end())
	node = &it->second;
    }
  return (node);
}

bool Parser::VisitEnter(const TiXmlElement& elem, const TiXmlAttribute* attr)
{
  const char* value;

  (void)attr;
  value = elem.Value();
  if (!strcmp(value, "dependency"))
    {
      const char* host_id1;
      const char* host_id2;
      const char* service_id1;
      const char* service_id2;

      host_id1 = elem.Attribute("dependent_host_id");
      host_id2 = elem.Attribute("host_id");
      service_id1 = elem.Attribute("dependent_service_id");
      service_id2 = elem.Attribute("service_id");
      if (host_id1 && host_id2)
        {
          Node* n1;
          Node* n2;

          n1 = this->FindNode(host_id1, service_id1);
          n2 = this->FindNode(host_id2, service_id2);
          if (n1 && n2)
            {
              n1->depends_on.push_back(n2);
              n2->depended_by.push_back(n1);
            }
        }
    }
  else if (!strcmp(value, "host"))
    {
      const char* id;

      id = elem.Attribute("id");
      if (id)
        {
          Node node;

          node.host_id = strtol(id, NULL, 0);
          (*this->hosts_)[node.host_id] = node;
        }
    }
  else if (!strcmp(value, "parent"))
    {
      const char* host;
      const char* parent;

      host = elem.Attribute("host");
      parent = elem.Attribute("parent");
      if (host && parent)
        {
          std::map<int, Node>::iterator it1;
          std::map<int, Node>::iterator it2;

          it1 = (*this->hosts_).find(strtol(host, NULL, 0));
          it2 = (*this->hosts_).find(strtol(parent, NULL, 0));
          if ((it1 != (*this->hosts_).end()) && (it2 != (*this->hosts_).end()))
            {
              it1->second.parents.push_back(&it2->second);
              it2->second.children.push_back(&it1->second);
            }
        }
    }
  else if (!strcmp(value, "service"))
    {
      const char* host;
      const char* id;

      host = elem.Attribute("host");
      id = elem.Attribute("id");
      if (host && id)
        {
          Node node;

          node.host_id = strtol(id, NULL, 0);
          node.service_id = strtol(id, NULL, 0);
          (*this->services_)[std::make_pair(node.host_id, node.service_id)] = node;
        }
    }
  return (true);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
Parser::Parser() {}

/**
 *  Copy constructor.
 *
 *  \param[in] parser Unused.
 */
Parser::Parser(const Parser& parser)
{
  (void)parser;
}

/**
 *  Destructor.
 */
Parser::~Parser() {}

/**
 *  Assignment operator.
 *
 *  \param[in] parser Unused.
 *
 *  \return *this
 */
Parser& Parser::operator=(const Parser& parser)
{
  (void)parser;
  return (*this);
}

/**
 *  Parse a configuration file.
 *
 *  \param[in]  filename Path to the correlation file.
 *  \param[out] hosts    List of hosts.
 *  \param[out] services List of services.
 */
void Parser::Parse(const char* filename,
                   std::map<int, Node>& hosts,
                   std::map<std::pair<int, int>, Node>& services)
{
  TiXmlDocument root;

  if (!root.LoadFile(filename))
    throw (Exception(0, "Could not load correlation file."));
  this->hosts_ = &hosts;
  this->services_ = &services;
  root.Accept(this);
  return ;
}
