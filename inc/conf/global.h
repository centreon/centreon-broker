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

#ifndef CONF_GLOBAL_H_
# define CONF_GLOBAL_H_

namespace             CentreonBroker
{
  namespace           Conf
  {
    /**
     *  \class GlobalConf global.h "conf/global.h"
     *  \brief Holds global configuration options.
     *
     *  Some variables can be defined globally. The GlobalConf structures holds
     *  all of these variables.
     */
    struct            GlobalConf
    {
      struct          GlobalOutputConf
      {
	unsigned int  connection_retry_interval;
	unsigned int  query_commit_interval;
	unsigned int  time_commit_interval;
      }               output;
    };

    extern GlobalConf global_conf;
    void              ResetGlobalConf() throw ();
  }
}

#endif /* !CONF_GLOBAL_H_ */
