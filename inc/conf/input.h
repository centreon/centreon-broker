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
    /**
     *  \class Input input.h "conf/input.h"
     *  \brief Holds configuration parameters of an input.
     *
     *  The Input class holds configuration parameters of an input source. This
     *  source can be of any type : Unix socket, IPv4 socket, ... Users should
     *  check its type before fetching arguments.
     *
     *  \see GetType
     */
    class                Input
    {
     public:
      enum               Type
      {
	UNKNOWN = 1,
	IPV4,
	IPV6,
	UNIX
      };
     private:
      std::string        name_;
      Type               type_;
      // IP specific
      std::string        interface_;
      unsigned short     port_;
      // Unix specific
      std::string        socket_path_;

     public:
                         Input();
                         Input(const Input& input);
                         ~Input();
      Input&             operator=(const Input& input);
      bool               operator==(const Input& input) const;
      bool               operator!=(const Input& input) const;
      bool               operator<(const Input& input) const;
      // Getters
      const std::string& GetIPInterface() const throw ();
      unsigned short     GetIPPort() const throw ();
      const std::string& GetName() const throw ();
      Type               GetType() const throw ();
      const std::string& GetUnixSocketPath() const throw ();
      // Setters
      void               SetIPInterface(const std::string& iface);
      void               SetIPPort(unsigned short port) throw ();
      void               SetName(const std::string& name);
      void               SetType(Type type) throw ();
      void               SetUnixSocketPath(const std::string& usp);
    };
  }
}

#endif /* !CONF_INPUT_H_ */
