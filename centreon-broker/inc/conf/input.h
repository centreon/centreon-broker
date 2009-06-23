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

#ifndef CONF_INPUT_H_
# define CONF_INPUT_H_

# include <string>

namespace                CentreonBroker
{
  namespace              Conf
  {
    class                Input
    {
     private:
      enum               UShort
      {
	PORT = 0,
	USHORT_NB
      };
      enum               String
      {
	TLS_CA = 0,
	TLS_CERTIFICATE,
	TLS_KEY,
	TYPE,
	STRING_NB
      };
      unsigned short     ushorts_[USHORT_NB];
      std::string        strings_[STRING_NB];

     public:
                         Input();
                         Input(const Input& input);
                         ~Input();
      Input&             operator=(const Input& input);
      bool               operator==(const Input& input);
      unsigned short     GetPort() const throw ();
      const std::string& GetTlsCa() const throw ();
      const std::string& GetTlsCertificate() const throw ();
      const std::string& GetTlsKey() const throw ();
      const std::string& GetType() const throw ();
      void               SetPort(unsigned short port) throw ();
      void               SetTlsCa(const std::string& ca);
      void               SetTlsCertificate(const std::string& certificate);
      void               SetTlsKey(const std::string& key);
      void               SetType(const std::string& type);
    };
  }
}

#endif /* !CONF_INPUT_H_ */
