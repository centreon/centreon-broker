/*
** Copyright 2011 Merethis
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

#include <assert.h>
#include <iostream>
#include <QScopedPointer>
#include <stdlib.h>
#include "config/applier/logger.hh"
#include "exceptions/basic.hh"
#include "logging/file.hh"
#include "logging/logging.hh"
#include "logging/ostream.hh"
#include "logging/syslogger.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

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
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] l Object to copy.
 */
logger::logger(logger const& l) {
  (void)l;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] l Object to copy.
 *
 *  @return This object.
 */
logger& logger::operator=(logger const& l) {
  (void)l;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Create a backend object from its configuration.
 *
 *  @param[in] cfg Logging backend configuration.
 *
 *  @return New logging backend.
 */
QSharedPointer<logging::backend> logger::_new_backend(config::logger const& cfg) {
  QSharedPointer<logging::backend> back;
  switch (cfg.type()) {
   case config::logger::file:
    {
      if (cfg.name().isEmpty())
	throw (exceptions::basic() << "attempt to log on an empty file");
      QScopedPointer<logging::file> file(new logging::file);
      file->open(cfg.name().toStdString().c_str());
      back = QSharedPointer<logging::backend>(file.data());
      file.take();
    }
    break ;
   case config::logger::standard:
    {
      std::ostream* out;
      if ((cfg.name() == "stderr") || (cfg.name() == "cerr"))
	out = &std::cerr;
      else if ((cfg.name() == "stdlog") || (cfg.name() == "clog"))
	out = &std::clog;
      else if ((cfg.name() == "stdout") || (cfg.name() == "cout"))
	out = &std::cout;
      else
	throw (exceptions::basic() << "attempt to log on an undefined output object");
      back = QSharedPointer<logging::backend>(new logging::ostream(*out));
    }
    break ;
   case config::logger::syslog:
    // XXX : handle facility
    back = QSharedPointer<logging::backend>(new logging::syslogger);
    break ;
   default:
    throw (exceptions::basic() << "attempt to create a logging object of unknown type");
  }

  return (back);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
logger::~logger() {}

/**
 *  Apply the configuration of a set of loggers.
 *
 *  @param[in] loggers List of loggers that should exist.
 */
void logger::apply(QList<config::logger> const& loggers) {
  // Find which loggers are already created,
  // which should be created
  // and which should be deleted.
  QList<config::logger> to_create;
  QMap<config::logger, QSharedPointer<logging::backend> > to_delete(_backends);
  QMap<config::logger, QSharedPointer<logging::backend> > to_keep;
  for (QList<config::logger>::const_iterator it = loggers.begin(),
         end = loggers.end();
       it != end;
       ++it) {
    QMap<config::logger, QSharedPointer<logging::backend> >::iterator backend(to_delete.find(*it));
    if (backend != to_delete.end()) {
      to_keep.insert(backend.key(), backend.value());
      to_delete.erase(backend);
    }
    else
      to_create.push_back(*it);
  }

  // Set the backends that will be kept.
  _backends = to_keep;

  // Remove loggers that do not exist anymore.
  for (QMap<config::logger, QSharedPointer<logging::backend> >::const_iterator it = to_delete.begin(),
         end = to_delete.end();
       it != end;
       ++it)
    logging::log_on(it.value().data(), 0, logging::NONE);

  // Free some memory.
  to_delete.clear();
  to_keep.clear();

  // Create new backends.
  for (QList<config::logger>::const_iterator it = to_create.begin(),
         end = to_create.end();
       it != end;
       ++it) {
    QSharedPointer<logging::backend> backend(_new_backend(*it));
    _backends[*it] = backend;
    logging::log_on(backend.data(), it->types(), it->level());
  }

  return ;
}

/**
 *  Get the class instance.
 *
 *  @return logger instance.
 */
logger& logger::instance() {
  static logger gl_logger;
  return (gl_logger);
}
