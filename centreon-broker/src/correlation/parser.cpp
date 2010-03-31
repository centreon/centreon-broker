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

bool Parser::VisitEnter(const TiXmlElement& elem, const TiXmlAttribute* attr)
{
  const char* value;

  value = elem.Value();
  if (!strcmp(value, "host"))
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
      const char* id;

      id = elem.Attribute("id");
      if (id)
        {
          Node node;

          node.service_id = strtol(id, NULL, 0);
          (*this->services_)[node.service_id] = node;
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
                   std::map<int, Node>& services)
{
  TiXmlDocument root;

  if (!root.LoadFile(filename))
    throw (Exception(0, "Could not load correlation file."));
  this->hosts_ = &hosts;
  this->services_ = &services;
  root.Accept(this);
  return ;
}
