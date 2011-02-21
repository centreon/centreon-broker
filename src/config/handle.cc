/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#include <algorithm>
#include <map>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include "config/factory.hh"
#include "config/globals.hh"
#include "config/handle.hh"
#include "config/parser.hh"
#include "exceptions/basic.hh"
#include "logging/logging.hh"
#include "multiplexing/publisher.hh"
#include "multiplexing/subscriber.hh"
#include "processing/failover_in.hh"
#include "processing/failover_out.hh"
#include "processing/feeder.hh"
#include "processing/listener.hh"

using namespace config;

/**************************************
*                                     *
*           Local Objects             *
*                                     *
**************************************/

// Configuration file.
static std::string gl_config_file;
// List of objects.
static std::map<config::interface, concurrency::thread*> gl_inputs;
static std::map<logger, logging::backend*>               gl_loggers;
static std::map<config::interface, concurrency::thread*> gl_outputs;
static std::list<concurrency::thread*>                   gl_erasable;

/**
 *  Add an erasable thread.
 */
static void add_erasable(concurrency::thread* t) {
  for (std::list<concurrency::thread*>::iterator it
         = gl_erasable.begin(),
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
static void config_update(int signum) {
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
void config::handle() {
  config::parser p;

  // Parse configuration file.
  p.parse(gl_config_file);

  // Remove loggers not present anymore.
  for (std::map<logger, logging::backend*>::iterator
         it = gl_loggers.begin(), end = gl_loggers.end();
       it != end;) {
    std::list<logger>::iterator l(
      std::find(p.loggers().begin(), p.loggers().end(), it->first));
    if (l != p.loggers().end()) {
      p.loggers().erase(l);
      ++it;
    }
    else {
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
       ++it) {
    logging::backend* obj(factory::build_logger(*it));
    if (obj) {
      gl_loggers[*it] = obj;
      logging::log_on(obj, it->types(), it->level());
    }
  }

  // Remove output objects not present anymore.
  for (std::map<config::interface, concurrency::thread*>::iterator
         it = gl_outputs.begin(), end = gl_outputs.end();
       it != end;) {
    std::list<interface>::iterator o(
      std::find(p.outputs().begin(), p.outputs().end(), it->first));
    if (o != p.outputs().end()) {
      p.outputs().erase(o);
      ++it;
    }
    else {
      std::map<config::interface, concurrency::thread*>::iterator to_erase(it);
      ++it;
      to_erase->second->exit();
      add_erasable(to_erase->second);
      gl_outputs.erase(to_erase);
    }
  }

  // Create output objects.
  for (std::list<interface>::iterator it = p.outputs().begin(),
         end = p.outputs().end();
       it != end;
       ++ it) {
    logging::debug << logging::MEDIUM << "Adding new output object.";
    if ((config::interface::ipv4_server == it->type)
        || (config::interface::ipv6_server == it->type)
        || (config::interface::unix_server == it->type)) {
      std::auto_ptr<io::acceptor> acceptor(
        config::factory::build_acceptor(*it));
      std::auto_ptr<processing::listener> listener(
        new processing::listener);
      listener->init(acceptor.get(),
                     ((it->protocol == config::interface::ndo)
                      ? processing::listener::NDO
                      : processing::listener::XML),
                     processing::listener::OUT,
                     NULL);
      acceptor.release();
      gl_outputs[*it] = listener.get();
      listener.release();
    }
    else {
      std::auto_ptr<multiplexing::subscriber> subscriber(
        new multiplexing::subscriber);
      std::auto_ptr<processing::failover_out> feeder(
        new processing::failover_out);
      feeder->run(subscriber.get(),
                  *it,
                  NULL);
      subscriber.release();
      gl_outputs[*it] = feeder.get();
      feeder.release();
    }
  }

  // Remove input objects not present anymore.
  for (std::map<config::interface, concurrency::thread*>::iterator
         it = gl_inputs.begin(), end = gl_inputs.end();
       it != end;) {
    std::list<interface>::iterator i(
      std::find(p.inputs().begin(), p.inputs().end(), it->first));
    if (i != p.inputs().end()) {
      p.inputs().erase(i);
      ++it;
    }
    else {
      std::map<config::interface, concurrency::thread*>::iterator to_erase(it);
      ++it;
      to_erase->second->exit();
      add_erasable(to_erase->second);
      gl_inputs.erase(to_erase);
    }
  }

  // Create input objects.
  for (std::list<interface>::iterator it = p.inputs().begin(),
         end = p.inputs().end();
       it != end;
       ++it) {
    logging::debug << logging::MEDIUM << "Adding new input object.";
    if ((config::interface::ipv4_server == it->type)
        || (config::interface::ipv6_server == it->type)
        || (config::interface::unix_server == it->type)) {
      std::auto_ptr<io::acceptor> acceptor(
        config::factory::build_acceptor(*it));
      std::auto_ptr<processing::listener> listener(
        new processing::listener);
      listener->init(acceptor.get(),
                     ((it->protocol == config::interface::ndo)
                      ? processing::listener::NDO
                      : processing::listener::XML),
                     processing::listener::IN,
                     NULL);
      acceptor.release();
      gl_inputs[*it] = listener.get();
      listener.release();
    }
    else {
      std::auto_ptr<multiplexing::publisher> publisher(
        new multiplexing::publisher);
      std::auto_ptr<processing::failover_in> feeder(
        new processing::failover_in);
      feeder->run(*it,
                  publisher.get(),
                  NULL);
      publisher.release();
      gl_inputs[*it] = feeder.get();
      feeder.release();
    }
  }

  // Register callback for runtime configuration update.
  logging::debug << logging::LOW << "registering callback for runtime "
                                    "configuration update"
                 << " (deactivated for now)";
  signal(SIGHUP, &config_update);
  return ;
}

/**
 *  Parse a new configuration file and take proper actions regarding this
 *  configuration.
 *
 *  @param[in] config_file XML configuration file.
 */
void config::handle(std::string const& config_file) {
  logging::config << logging::MEDIUM << "setting new configuration file \""
                  << config_file.c_str() << "\"";
  gl_config_file = config_file;
  atexit(&config::unload);
  handle();
  if (globals::correlation)
    multiplexing::publisher::correlate();
  return ;
}

/**
 *  Reap terminated threads.
 */
void config::reap() {
  for (std::list<concurrency::thread*>::iterator
         it = gl_erasable.begin(),
         end = gl_erasable.end();
       it != end;
       ++it)
    try {
      delete *it;
    }
    catch (exceptions::basic const& e) {
    }
  gl_erasable.clear();
  return ;
}

/**
 *  Unload configuration objects.
 */
void config::unload() {
  logging::config << logging::HIGH << "unloading configuration";
  std::map<config::interface, concurrency::thread*>::iterator it, end;

  /* Close input sources. */
  for (it = gl_inputs.begin(), end = gl_inputs.end(); it != end; ++it)
    try {
      it->second->exit();
    }
    catch (exceptions::basic const& e) {}

  /* Wait for input sources to terminate. */
  for (it = gl_inputs.begin(), end = gl_inputs.end(); it != end; ++it) {
    try {
      it->second->join();
    }
    catch (exceptions::basic const& e) {
      try {
        it->second->cancel();
      }
      catch (exceptions::basic const& e) {}
    }
    delete it->second;
  }
  gl_inputs.clear();

  /* Close output destinations. */
  for (it = gl_outputs.begin(), end = gl_outputs.end(); it != end; ++it)
    try {
      it->second->exit();
    }
    catch (exceptions::basic const& e) {}

  /* Wait for output destinations to terminate. */
  for (it = gl_outputs.begin(), end = gl_outputs.end(); it != end; ++it) {
    try {
      it->second->join();
    }
    catch (exceptions::basic const& e) {
      try {
        it->second->cancel();
      }
      catch (exceptions::basic const& e) {}
    }
    delete it->second;
  }
  gl_outputs.clear();

  /* Reap remaining threads. */
  config::reap();

  /* Closing log objects. */
  for (std::map<logger, logging::backend*>::iterator
         it2 = gl_loggers.begin(),
         end2 = gl_loggers.end();
       it2 != end2;
       ++it2)
    delete it->second;
  gl_loggers.clear();

  return ;
}
