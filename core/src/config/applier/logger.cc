/*
** Copyright 2011-2012,2015 Merethis
**
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
*/

#include <cstdlib>
#include <iostream>
#include <memory>
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"
#ifdef CBMOD
#  include "com/centreon/broker/neb/monitoring_logger.hh"
#endif // CBMOD
#include "com/centreon/broker/logging/syslogger.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

// Class instance.
static config::applier::logger* gl_logger = NULL;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
logger::~logger() {
  logging::debug(logging::high) << "log applier: destruction";
}

/**
 *  Apply the configuration of a set of loggers.
 *
 *  @param[in] loggers List of loggers that should exist.
 */
void logger::apply(std::list<config::logger> const& loggers) {
  // Log message.
  logging::config(logging::high) << "log applier: applying "
    << loggers.size() << " logging objects";

  // Find which loggers are already created,
  // which should be created
  // and which should be deleted.
  std::list<config::logger> to_create;
  std::map<config::logger, misc::shared_ptr<logging::backend> >
    to_delete(_backends);
  std::map<config::logger, misc::shared_ptr<logging::backend> >
    to_keep;
  for (std::list<config::logger>::const_iterator it = loggers.begin(),
         end = loggers.end();
       it != end;
       ++it) {
    std::map<config::logger, misc::shared_ptr<logging::backend> >::iterator
      backend(to_delete.find(*it));
    if (backend != to_delete.end()) {
      to_keep.insert(*backend);
      to_delete.erase(backend);
    }
    else
      to_create.push_back(*it);
  }

  // Set the backends that will be kept.
  _backends = to_keep;

  // Remove loggers that do not exist anymore.
  for (std::map<config::logger, misc::shared_ptr<logging::backend> >::const_iterator
         it(to_delete.begin()),
         end(to_delete.end());
       it != end;
       ++it)
    logging::manager::instance().log_on(*it->second, 0, logging::none);

  // Free some memory.
  to_delete.clear();
  to_keep.clear();

  // Create new backends.
  for (std::list<config::logger>::const_iterator
         it(to_create.begin()),
         end(to_create.end());
       it != end;
       ++it) {
    logging::config(logging::medium)
      << "log applier: creating new logger";
    misc::shared_ptr<logging::backend> backend(_new_backend(*it));
    _backends[*it] = backend;
    logging::manager::instance().log_on(
      *backend,
      it->types(),
      it->level());
  }

  return ;
}

/**
 *  Get the class instance.
 *
 *  @return logger instance.
 */
logger& logger::instance() {
  return (*gl_logger);
}

/**
 *  Load the singleton.
 */
void logger::load() {
  if (!gl_logger)
    gl_logger = new logger;
  return ;
}

/**
 *  Unload the singleton.
 */
void logger::unload() {
  delete gl_logger;
  gl_logger = NULL;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
logger::logger() {}

/**
 *  Create a backend object from its configuration.
 *
 *  @param[in] cfg Logging backend configuration.
 *
 *  @return New logging backend.
 */
misc::shared_ptr<logging::backend> logger::_new_backend(config::logger const& cfg) {
  misc::shared_ptr<logging::backend> back;
  switch (cfg.type()) {
  case config::logger::file:
    {
      if (cfg.name().isEmpty())
        throw (exceptions::msg()
               << "log applier: attempt to log on an empty file");
      std::auto_ptr<logging::file>
        file(new logging::file(cfg.name(), cfg.max_size()));
      back = misc::shared_ptr<logging::backend>(file.get());
      file.release();
    }
    break ;
  case config::logger::monitoring:
    {
#ifdef CBMOD
      std::auto_ptr<neb::monitoring_logger>
        monitoring(new neb::monitoring_logger);
      back = misc::shared_ptr<logging::backend>(monitoring.get());
      monitoring.release();
#else
      logging::info(logging::high) << "log applier: monitoring"
        " logger type is not supported in standalone mode";
#endif // CBMOD
    }
    break ;
  case config::logger::standard:
    {
      FILE* out;
      if ((cfg.name() == "stderr") || (cfg.name() == "cerr"))
        out = stderr;
      else if ((cfg.name() == "stdout") || (cfg.name() == "cout"))
        out = stdout;
      else
        throw (exceptions::msg() << "log applier: attempt to log on " \
                 "an undefined output object");
      back = misc::shared_ptr<logging::backend>(new logging::file(out));
    }
    break ;
  case config::logger::syslog:
    back = misc::shared_ptr<logging::backend>(
                   new logging::syslogger(cfg.facility()));
    break ;
  default:
    throw (exceptions::msg() << "log applier: attempt to create a " \
             "logging object of unknown type");
  }

  return (back);
}
