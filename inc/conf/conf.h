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

#ifndef CONF_H_
# define CONF_H_

# include <fstream>
# include <map>
# include <string>
# include <vector>
# include "conf/input.h"
# include "conf/log.h"
# include "conf/output.h"

namespace                 CentreonBroker
{
  namespace               Conf
  {
    class                 Conf
    {
     private:
      std::string         filename_;
      std::vector<Input>  inputs_;
      mutable std::vector<Input>::const_iterator
        in_it_;
      std::vector<Log>    logs_;
      mutable std::vector<Log>::const_iterator
	log_it_;
      std::vector<Output> outputs_;
      mutable std::vector<Output>::const_iterator
	out_it_;
      std::map<std::string, std::string>
        params_;
      void                HandleInput(std::ifstream& ifs);
      void                HandleLog(std::ifstream& ifs);
      void                HandleOutput(std::ifstream& ifs);

     public:
                          Conf();
                          Conf(const Conf& conf);
                          ~Conf();
      Conf&               operator=(const Conf& conf);
      int                 GetIntegerParam(const std::string& param) const;
      const std::string*  GetStringParam(const std::string& param) const;
      const Input*        GetNextInput() const;
      const Log*          GetNextLog() const;
      const Output*       GetNextOutput() const;
      void                Load(const std::string& filename);
      void                Update();
    };
  }
}

#endif /* !CONF_H_ */
