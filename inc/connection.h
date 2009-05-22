/*
** connection.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 05/22/09 Matthieu Kermagoret
*/

#ifndef CONNECTION_H_
# define CONNECTION_H_

# include <string>
# include <sys/types.h>
# include "connection_status.h"

namespace              CentreonBroker
{
  class                Connection : public ConnectionStatus
  {
   private:
    enum               String
    {
      AGENT_NAME = 0,
      AGENT_VERSION,
      CONNECT_SOURCE,
      CONNECT_TYPE,
      STRING_NB
    };
    enum               TimeT
    {
      CONNECT_TIME = 0,
      DATA_START_TIME,
      TIMET_NB
    };
    std::string        strings_[STRING_NB];
    time_t             timets_[TIMET_NB];
    void               InternalCopy(const Connection& c);

   public:
                       Connection() throw ();
                       Connection(const Connection& c);
                       ~Connection() throw ();
    Connection&        operator=(const Connection& c);
    // Getters
    const std::string& GetAgentName() const throw ();
    const std::string& GetAgentVersion() const throw ();
    const std::string& GetConnectSource() const throw ();
    time_t             GetConnectTime() const throw ();
    const std::string& GetConnectType() const throw ();
    time_t             GetDataStartTime() const throw ();
    int                GetType() const throw ();
    // Setters
    void               SetAgentName(const std::string& an);
    void               SetAgentVersion(const std::string& av);
    void               SetConnectSource(const std::string& cs);
    void               SetConnectTime(time_t ct) throw ();
    void               SetConnectType(const std::string& ct);
    void               SetDataStartTime(time_t dst) throw ();
  };
}

#endif /* !CONNECTION_H_ */
