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

#include "correlation/parser.h"

using namespace Correlation;

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
}
