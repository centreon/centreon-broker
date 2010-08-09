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

#include <algorithm>
#include <map>
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
// List of objects.
static std::map<interface, Concurrency::Thread*> gl_inputs;
static std::map<logger, logging::backend*>       gl_loggers;
static std::map<interface, Concurrency::Thread*> gl_outputs;
static std::list<Concurrency::Thread*>           gl_erasable;

/**
 *  Add an erasable thread.
 */
static void add_erasable(Concurrency::Thread* t)
{
  for (std::list<Concurrency::Thread*>::iterator it = gl_erasable.begin(),
         end = gl_erasable.end();
       it != end;
       ++it)
    delete (*it);
  gl_erasable.push_back(t);
  return ;
}

/**
 *  Configuration-update signal callback.
 *
 *  @param[in] signum Unused.
 */
static void config_update(int signum)
{
  (void)signum;
  handle();
  signal(SIGHUP, &config_update);
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

  // Remove loggers not present anymore.
  for (std::map<logger, logging::backend*>::iterator
         it = gl_loggers.begin(), end = gl_loggers.end();
       it != end;)
    {
      std::list<logger>::iterator l;

      if ((l = std::find(p.loggers().begin(), p.loggers().end(), it->first))
          != p.loggers().end())
        {
          p.loggers().erase(l);
          ++it;
        }
      else
        {
          std::map<logger, logging::backend*>::iterator to_erase(it);

          ++it;
          logging::log_on(to_erase->second, 0, logging::NONE);
          gl_loggers.erase(to_erase);
        }
    }

  // Create new logger objects.
  for (std::list<logger>::iterator it = p.loggers().begin(),
         end = p.loggers().end();
       it != end;
       ++it)
    {
      logging::backend* obj;

      obj = factory::build_logger(*it);
      if (obj)
        {
          gl_loggers[*it] = obj;
          logging::log_on(obj, it->types(), it->level());
        }
    }

  // Remove output objects not present anymore.
  for (std::map<interface, Concurrency::Thread*>::iterator
         it = gl_outputs.begin(), end = gl_outputs.end();
       it != end;)
    {
      std::list<interface>::iterator o;

      if ((o = std::find(p.outputs().begin(), p.outputs().end(), it->first))
          != p.outputs().end())
        {
          p.outputs().erase(o);
          ++it;
        }
      else
        {
          std::map<interface, Concurrency::Thread*>::iterator to_erase(it);

          ++it;
          to_erase->second->Exit();
          add_erasable(to_erase->second);
          gl_outputs.erase(to_erase);
        }
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
          gl_outputs[*it] = listener.get();
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
          gl_outputs[*it] = feeder.get();
          feeder.release();
        }
    }

  // Remove input objects not present anymore.
  for (std::map<interface, Concurrency::Thread*>::iterator
         it = gl_inputs.begin(), end = gl_inputs.end();
       it != end;)
    {
      std::list<interface>::iterator i;

      if ((i = std::find(p.inputs().begin(), p.inputs().end(), it->first))
          != p.inputs().end())
        {
          p.inputs().erase(i);
          ++it;
        }
      else
        {
          std::map<interface, Concurrency::Thread*>::iterator to_erase(it);

          ++it;
          to_erase->second->Exit();
          add_erasable(to_erase->second);
          gl_inputs.erase(to_erase);
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
          gl_inputs[*it] = listener.get();
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
          gl_inputs[*it] = feeder.get();
          feeder.release();
        }
    }

  // Register callback for runtime configuration update.
  logging::debug << logging::LOW << "Registering callback for runtime "
                                    "configuration update.";
  logging::debug << logging::LOW << "(deactivated for now)";
  signal(SIGHUP, &config_update);
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
