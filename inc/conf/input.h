/*
** input.h for CentreonBroker in ./inc/conf
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/17/09 Matthieu Kermagoret
** Last update 06/17/09 Matthieu Kermagoret
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
      enum               Bool
      {
	TLS = 0,
	BOOL_NB
      };
      enum               UShort
      {
	PORT = 0,
	USHORT_NB
      };
      enum               String
      {
	TYPE = 0,
	STRING_NB
      };
      bool               bools_[BOOL_NB];
      unsigned short     ushorts_[USHORT_NB];
      std::string        strings_[STRING_NB];

     public:
                         Input();
                         Input(const Input& input);
                         ~Input();
      Input&             operator=(const Input& input);
      bool               operator==(const Input& input);
      unsigned short     GetPort() const throw ();
      bool               GetTls() const throw ();
      const std::string& GetType() const throw ();
      void               SetPort(unsigned short port) throw ();
      void               SetTls(bool tls) throw ();
      void               SetType(const std::string& type);
    };
  }
}

#endif /* !CONF_INPUT_H_ */
