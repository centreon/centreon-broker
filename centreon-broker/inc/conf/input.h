/*
** input.h for CentreonBroker in ./inc/conf
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/17/09 Matthieu Kermagoret
** Last update 06/19/09 Matthieu Kermagoret
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
	TLS_DH512,
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
      const std::string& GetTlsDH512() const throw ();
      const std::string& GetTlsKey() const throw ();
      const std::string& GetType() const throw ();
      void               SetPort(unsigned short port) throw ();
      void               SetTlsCa(const std::string& ca);
      void               SetTlsCertificate(const std::string& certificate);
      void               SetTlsDH512(const std::string& dh512);
      void               SetTlsKey(const std::string& key);
      void               SetType(const std::string& type);
    };
  }
}

#endif /* !CONF_INPUT_H_ */
