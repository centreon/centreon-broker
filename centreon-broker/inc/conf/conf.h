/*
** conf.h for CentreonBroker in ./inc/conf
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/17/09 Matthieu Kermagoret
** Last update 06/22/09 Matthieu Kermagoret
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
