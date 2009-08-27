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

#ifndef CONF_OUTPUT_H_
# define CONF_OUTPUT_H_

# include <string>

namespace                CentreonBroker
{
  namespace              Conf
  {
    /**
     *  \class Output output.h "conf/output.h"
     *  \brief Holds configuration parameters of an output.
     *
     *  The Output class holds configuration parameters of an output source.
     *  Currently only database outputs are supported.
     */
    class                Output
    {
     public:
      enum               Type
      {
	UNKNOWN = 1,
	MYSQL,
	ORACLE,
	POSTGRESQL
      };

     private:
      unsigned int       connection_retry_interval_;
      std::string        db_;
      std::string        dumpfile_;
      std::string        host_;
      std::string        name_;
      std::string        password_;
      unsigned int       query_commit_interval_;
      unsigned int       time_commit_interval_;
      Type               type_;
      std::string        user_;

     public:
                         Output();
                         Output(const Output& output);
                         ~Output();
      Output&            operator=(const Output& output);
      bool               operator==(const Output& output) const;
      bool               operator!=(const Output& output) const;
      bool               operator<(const Output& output) const;
      // Getters
      unsigned int       GetConnectionRetryInterval() const throw ();
      const std::string& GetDB() const throw ();
      const std::string& GetDumpFile() const throw ();
      const std::string& GetHost() const throw ();
      const std::string& GetName() const throw ();
      const std::string& GetPassword() const throw ();
      unsigned int       GetQueryCommitInterval() const throw ();
      unsigned int       GetTimeCommitInterval() const throw ();
      Type               GetType() const throw ();
      const std::string& GetUser() const throw ();
      // Setters
      void               SetConnectionRetryInterval(unsigned int cri) throw ();
      void               SetDB(const std::string& db);
      void               SetDumpFile(const std::string& df);
      void               SetHost(const std::string& host);
      void               SetName(const std::string& name);
      void               SetPassword(const std::string& password);
      void               SetQueryCommitInterval(unsigned int qci) throw ();
      void               SetTimeCommitInterval(unsigned int tci) throw ();
      void               SetType(Type type) throw ();
      void               SetUser(const std::string& user);
    };
  }
}

#endif /* !CONF_OUTPUT_H_ */
