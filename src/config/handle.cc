/*
**  Copyright 2010 MERETHIS
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

#include <signal.h>
#include <string>
#include "config/handle.hh"
#include "config/factory.hh"
#include "config/parser.hh"
#include "logging/logging.hh"

using namespace config;

/**************************************
*                                     *
*           Local Objects             *
*                                     *
**************************************/

// Configuration file.
static std::string gl_config_file;

/**
 *  Configuration-update signal callback.
 *
 *  @param[in] signum Unused.
 */
static void config_update(int signum)
{
  (void)signum;
  handle();
  return ;
}

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

/**
 *  Parse the previous configuration file and take proper actions regarding
 *  this configuration.
 */
void config::handle()
{
  config::parser p;

  // Parse configuration file.
  p.parse(gl_config_file);

  // Create logger objects.
  for (std::list<logger>::iterator it = p.loggers().begin(),
         end = p.loggers().end();
       it != end;
       ++it)
    {
      logging::backend* obj;

      obj = factory::build(*it);
      if (obj)
	logging::log_on(obj, it->types(), it->level());
    }

  // Register callback for runtime configuration update.
  logging::debug << logging::LOW << "Registering callback for runtime "
                                    "configuration update.";
  logging::debug << logging::LOW << "(deactivated for now)";
  //signal(SIGHUP, config_update);
  return ;
}

/**
 *  Parse a new configuration file and take proper actions regarding this
 *  configuration.
 *
 *  @param[in] config_file XML configuration file.
 */
void config::handle(std::string const& config_file)
{
  logging::config << logging::MEDIUM << "Setting new configuration file \""
                  << config_file.c_str() << "\"";
  gl_config_file = config_file;
  handle();
  return ;
}
