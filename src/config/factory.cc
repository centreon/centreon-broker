/*
** This file is part of Centreon Dhana.
**
** Centreon Dhana is free software: you can redistribute it and/or modify it
** under the terms of the GNU Affero General Public License as published by the
** Free Software Foundation, either version 3 of the License, or (at your
** option) any later version.
**
** Centreon Dhana is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public
** License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with Centreon Dhana. If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <memory>
#include <strings.h>
#include "config/factory.hh"
#include "exceptions/basic.hh"
#include "logging/file.hh"
#include "logging/logging.hh"
#include "logging/ostream.hh"
#include "logging/syslogger.hh"

using namespace config;

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

/**
 *  Build a logging backend from a configuration.
 *
 *  @return Backend object.
 */
logging::backend* factory::build(logger const& conf)
{
  logging::backend* obj;

  switch (conf.type())
    {
    case logger::file:
      {
	std::auto_ptr<logging::file> file(new logging::file);
	file->open(conf.name().c_str());
	obj = file.release();
      }
      break ;
    case logger::standard:
      {
	char const* name;
	std::ostream* ostr;

	name = conf.name().c_str();
	if (!strcasecmp(name, "stdout") || !strcasecmp(name, "cout"))
	  ostr = &std::cout;
	else if (!strcasecmp(name, "stderr") || !strcasecmp(name, "cerr"))
	  ostr = &std::cerr;
	else if (!strcasecmp(name, "stdlog") || !strcasecmp(name, "clog"))
	  ostr = &std::clog;
	else
	  throw (exceptions::basic() << "Invalid standard name: " << name);
	obj = new logging::ostream(*ostr);
      }
      break ;
    case logger::syslog:
      obj = new logging::syslogger();
      break ;
    default:
      obj = NULL;
      logging::config << logging::HIGH << "Ignoring logger with unknown type.";
    }
  return (obj);
}
