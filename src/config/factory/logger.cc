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

#include <iostream>
#include <memory>
#include <strings.h>
#include "config/factory.hh"
#include "config/logger.hh"
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
logging::backend* factory::build_logger(logger const& conf) {
  logging::backend* obj;
  switch (conf.type()) {
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
        throw (exceptions::basic() << "invalid standard name: " << name);
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
