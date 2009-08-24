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

#include "conf/global.h"

using namespace CentreonBroker::Conf;

/**************************************
*                                     *
*            Static Object            *
*                                     *
**************************************/

GlobalConf CentreonBroker::Conf::global_conf =
  {
    // Global output configuration
    {
      30,    // Connection retry interval, in seconds.
      10000, // Maximum number of queries that can be executed before a
             // transaction commit occurs.
      20     // Maximum amount of time in seconds that can elapse before a
             // transaction commit occurs.
    }
  };

/**************************************
*                                     *
*           Static Function           *
*                                     *
**************************************/

/**
 *  Reset the global configuration to its defaults.
 */
void CentreonBroker::Conf::ResetGlobalConf() throw ()
{
  global_conf.output.connection_retry_interval = 30;
  global_conf.output.query_commit_interval     = 10000;
  global_conf.output.time_commit_interval      = 20;
  return ;
}
