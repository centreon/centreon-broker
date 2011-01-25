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

#include "exceptions/retval.hh"
#include "init.hh"
#include "interface/db/internal.hh"
#include "interface/ndo/internal.hh"
#include "interface/xml/internal.hh"
#ifdef USE_TLS
# include "io/tls/internal.hh"
#endif /* !USE_TLS */
#include "logging/logging.hh"

/**************************************
*                                     *
*           Global Function           *
*                                     *
**************************************/

/**
 *  Unload everything loaded by init().
 */
void deinit() {
  // Unload configuration.
  logging::debug << logging::MEDIUM << "unloading configuration";
  // XXX

#ifdef USE_TLS
  // Unload GNU TLS library.
  logging::debug << logging::MEDIUM << "unloading GNU TLS library";
  io::tls::destroy();
#endif /* USE_TLS */

  logging::clear();

  return ;
}

/**
 *  Load everything necessary for the program to work.
 */
void init() {
#ifdef USE_TLS
  // Initialize GNU TLS.
  logging::debug << logging::MEDIUM << "initializing GNU TLS library";
  io::tls::initialize();
#endif /* USE_TLS */

  // Initialize all interface objects.
  logging::debug << logging::MEDIUM << "initializing DB engine";
  interface::db::initialize();
  logging::debug << logging::MEDIUM << "initializing NDO engine";
  interface::ndo::initialize();
  logging::debug << logging::MEDIUM << "initializing XML engine";
  interface::xml::initialize();

  return ;
}
