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
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"
#include "processing/failover_in.h"
#include "processing/failover_out.h"
#include "processing/feeder.h"
#include "processing/listener.h"

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

      obj = factory::build_logger(*it);
      if (obj)
	logging::log_on(obj, it->types(), it->level());
    }

  // Create output objects.
  for (std::list<interface>::iterator it = p.outputs().begin(),
         end = p.outputs().end();
       it != end;
       ++ it)
    {
      logging::debug << logging::MEDIUM << "Adding new output object.";
      if ((config::interface::ipv4_server == it->type)
	  || (config::interface::ipv6_server == it->type)
	  || (config::interface::unix_server == it->type))
        {
          std::auto_ptr<IO::Acceptor> acceptor(
            config::factory::build_acceptor(*it));
          std::auto_ptr<Processing::Listener> listener(
            new Processing::Listener);

          listener->Init(acceptor.get(),
                         ((it->protocol == config::interface::ndo)
                          ? Processing::Listener::NDO
                          : Processing::Listener::XML),
                         Processing::Listener::OUT,
                         NULL);
          acceptor.release();
          listener.release();
        }
      else
        {
          std::auto_ptr<Processing::FailoverOut> feeder;
          std::auto_ptr<Multiplexing::Subscriber> subscriber;

          subscriber.reset(new Multiplexing::Subscriber);
          feeder.reset(new Processing::FailoverOut);
          feeder->Run(subscriber.get(),
                      *it,
                      NULL);
          subscriber.release();
          feeder.release();
        }
    }

  // Create input objects.
  for (std::list<interface>::iterator it = p.inputs().begin(),
	 end = p.inputs().end();
       it != end;
       ++it)
    {
      logging::debug << logging::MEDIUM << "Adding new input object.";
      if ((config::interface::ipv4_server == it->type)
          || (config::interface::ipv6_server == it->type)
          || (config::interface::unix_server == it->type))
        {
          std::auto_ptr<IO::Acceptor> acceptor(
            config::factory::build_acceptor(*it));
          std::auto_ptr<Processing::Listener> listener(
            new Processing::Listener);

          listener->Init(acceptor.get(),
                         ((it->protocol == config::interface::ndo)
                          ? Processing::Listener::NDO
                          : Processing::Listener::XML),
                         Processing::Listener::IN,
                         NULL);
          acceptor.release();
          listener.release();
        }
      else
        {
          std::auto_ptr<Processing::FailoverIn> feeder;
          std::auto_ptr<Multiplexing::Publisher> publisher;

          publisher.reset(new Multiplexing::Publisher);
          feeder.reset(new Processing::FailoverIn);
          feeder->Run(*it,
                      publisher.get(),
                      NULL);
          publisher.release();
          feeder.release();
        }
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
