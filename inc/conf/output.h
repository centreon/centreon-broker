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
    class                Output
    {
     private:
      enum               String
      {
	DB = 0,
	HOST,
	PASSWORD,
	PREFIX,
	TYPE,
	USER,
	STRING_NB
      };
      std::string        strings_[STRING_NB];

     public:
                         Output();
                         Output(const Output& output);
                         ~Output();
      Output&            operator=(const Output& output);
      bool               operator==(const Output& output);
      const std::string& GetDb() const throw ();
      const std::string& GetHost() const throw ();
      const std::string& GetPassword() const throw ();
      const std::string& GetPrefix() const throw ();
      const std::string& GetType() const throw ();
      const std::string& GetUser() const throw ();
      void               SetDb(const std::string& db);
      void               SetHost(const std::string& host);
      void               SetPassword(const std::string& password);
      void               SetPrefix(const std::string& prefix);
      void               SetType(const std::string& type);
      void               SetUser(const std::string& user);
    };
  }
}

#endif /* !CONF_OUTPUT_H_ */
