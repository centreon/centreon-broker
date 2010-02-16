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

#ifdef USE_MYSQL
# include <mysql.h>                // for mysql_library_init
#endif /* !USE_MYSQL */
#include "configuration/manager.h"
#include "exception.h"
#include "init.h"
#include "interface/ndo/internal.h"
#include "interface/xml/destination.h"
#ifdef USE_TLS
# include "io/tls/internal.h"
#endif /* !USE_TLS */
#include "logging.h"
#include "mapping.h"

/**************************************
*                                     *
*           Global Function           *
*                                     *
**************************************/

/**
 *  Unload everything loaded by Init().
 */
void Deinit()
{
  // Unload configuration.
  LOGDEBUG("Unloading configuration ...");
  Configuration::Manager::Instance().Close();

  // Destroy O/R mapping.
  LOGDEBUG("Unloading DB engine ...");
  // XXX : MappingsDestroy();

#ifdef USE_TLS
  // Unload GNU TLS library.
  LOGDEBUG("Unloading GNU TLS library ...");
  IO::TLS::Destroy();
#endif /* USE_TLS */

#ifdef USE_MYSQL
  // Unload MySQL library.
  LOGDEBUG("Unloading MySQL library ...");
  mysql_library_end();
#endif /* USE_MYSQL */

  return ;
}

/**
 *  Load everything necessary for the program to work.
 */
void Init()
{
#ifdef USE_MYSQL
  // Initialize MySQL library.
  LOGDEBUG("Initializing MySQL library ...");
  if (mysql_library_init(0, NULL, NULL))
    throw (Exception(0, "MySQL library initialization failed."));
#endif /* USE_MYSQL */

#ifdef USE_TLS
  // Initialize GNU TLS.
  LOGDEBUG("Initializing GNU TLS library ...");
  IO::TLS::Initialize();
#endif /* USE_TLS */

  // Initialize all interface objects.
  LOGDEBUG("Initializing DB engine ...");
  //MappingsInit();
  LOGDEBUG("Initializing NDO engine ...");
  Interface::NDO::Initialize();
  LOGDEBUG("Initializing XML engine ...");
  Interface::XML::Destination::Initialize();

  return ;
}
